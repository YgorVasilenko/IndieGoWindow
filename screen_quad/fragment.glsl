#version 450 core

layout(binding = 0) uniform sampler2D overlaySampler;

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 fragTexCoord;

void main() {
    FragColor = texture(overlaySampler, fragTexCoord);
    if (FragColor.a == 0.f) {
        FragColor = vec4(0.1f, 0.1f, 0.18f, 1.0f);
    }
}