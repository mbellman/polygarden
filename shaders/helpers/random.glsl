float getRandom(float seed) {
  return fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec2 getRandomOffset(float seed) {
  float x = getRandom(seed) - 0.5;
  float y = getRandom(seed * 2.0) - 0.5;

  return normalize(vec2(x, y));
}