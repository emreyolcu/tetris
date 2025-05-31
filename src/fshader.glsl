varying vec4 color;

uniform int TextSwitch;
uniform vec4 uColor;

void main(void) {
  if (TextSwitch == 0) {
    gl_FragColor = color;
  } else {
    gl_FragColor = uColor;
  }
}
