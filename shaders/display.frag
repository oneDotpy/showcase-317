#version 330 core

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D displayTexture;

void main() {
    // Simple Y-flip for OpenGL texture coordinates
    vec2 texCoord = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);
    fragColor = texture(displayTexture, texCoord);
}
