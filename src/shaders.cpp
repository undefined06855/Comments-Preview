#include "shaders.hpp"

const GLchar* g_fadeShaderVertex = R"(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying mediump vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main() {
    gl_Position = CC_MVPMatrix * a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
)";

const GLchar* g_fadeShaderFragment = R"(
#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform sampler2D CC_Texture0;

void main() {
    // this is because we shoved it into the colour instead of like using a uniform like a normal person
    // if anyone can figure out why the uniform just cant be set let me know
    float width = floor(v_fragmentColor.r * 255.0 + 0.5) + floor(v_fragmentColor.g * 255.0 + 0.5) * 256.0;
    float height = floor(v_fragmentColor.a * 255.0 + 0.5) + floor(v_fragmentColor.b * 255.0 + 0.5) * 256.0;

    gl_FragColor = vec4(0.0);

    // add a black outline
    vec2 offset = vec2(0.5 / width, 0.5 / height);
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2(-1.0, -1.0)).a;
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2( 0.0, -1.0)).a;
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2( 1.0, -1.0)).a;
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2(-1.0,  0.0)).a;
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2( 0.0,  0.0)).a;
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2( 1.0,  0.0)).a;
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2(-1.0,  1.0)).a;
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2( 0.0,  1.0)).a;
    gl_FragColor.a += texture2D(CC_Texture0, v_texCoord + offset * vec2( 1.0,  1.0)).a;

    // fade 30 units on the right edge
    float fadeStart = width - 30.0;
    float fadeEnd = width - 5.0;
    float alpha = 1.0 - smoothstep(fadeStart, fadeEnd, v_texCoord.x * width);

    // fade 5 units on the left edge
    fadeStart = 0.0;
    fadeEnd = 5.0;
    alpha *= smoothstep(fadeStart, fadeEnd, v_texCoord.x * width);

    gl_FragColor += texture2D(CC_Texture0, v_texCoord);
    gl_FragColor.a *= alpha;
}
)";

bool g_fadeShaderFailedToLoad = false;

void loadShaders() {
    auto cache = cocos2d::CCShaderCache::sharedShaderCache();
    if (cache->programForKey("fade_shader"_spr)) return;

    auto program = new cocos2d::CCGLProgram;
    bool ret = program->initWithVertexShaderByteArray(g_fadeShaderVertex, g_fadeShaderFragment);
    if (!ret) {
        geode::log::warn("uh oh whoopsie poopsies yo shader failed");
        g_fadeShaderFailedToLoad = true;
        return;
    }

    program->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
    program->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);
    program->addAttribute(kCCAttributeNameColor, cocos2d::kCCVertexAttrib_Color);

    program->link();
    program->updateUniforms();

    geode::log::info("now you can see brainrotted comments two clicks faster!");

    cache->addProgram(program, "fade_shader"_spr);
}

$on_game(Loaded) { loadShaders(); }
$on_game(TexturesLoaded) { loadShaders(); }
