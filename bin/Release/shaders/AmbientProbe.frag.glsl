#version 450 core

out vec4 FragColor;
out vec4 BrightFragColor;

in VS_OUT{
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
  mat3 TBN;
  mat3 TBN2;
  vec3 wFragPos;
} fs_in;

const float PI = 3.14159;

//Spherical harmonics pre calculated values
const float Pi = 3.141592654f;
const float CosineA0 = Pi;
const float CosineA1 = (2.0 * Pi) / 3.0;
const float CosineA2 = Pi * 0.25;
const float C1 = 0.429043;
const float C2 = 0.511664;
const float C3 = 0.743125;
const float C4 = 0.886227;
const float C5 = 0.247708;


// scale for restored amount of lighting
struct SphericalHarmonicsFormat {
  vec3 u_L00; float padding0;
  vec3 u_L1m1; float padding1;
  vec3 u_L10; float padding2;
  vec3 u_L11; float padding3;
  vec3 u_L2m2; float padding4;
  vec3 u_L2m1; float padding5;
  vec3 u_L20; float padding6;
  vec3 u_L21; float padding7;
  vec3 u_L22; float padding8;
};


layout(std430, binding = 1) buffer AmbientLightSSBO
{
    SphericalHarmonicsFormat ambientProbes[7][7][7];

} lp;

int WIDTH = 7;
int DEPTH = 7;

vec3 sphericalHarmonics(vec3 N, ivec3 index)
{
   ivec3 i = clamp(index, ivec3(0), ivec3(6));
   return
      // band 0, constant value, details of lowest frequency
      C4 * lp.ambientProbes[i.x][i.y][i.z].u_L00 +

      // band 1, oriented along main axes
      2.0 * C2 * lp.ambientProbes[i.x][i.y][i.z].u_L11 * N.x +
      2.0 * C2 * lp.ambientProbes[i.x][i.y][i.z].u_L1m1 * N.y +
      2.0 * C2 * lp.ambientProbes[i.x][i.y][i.z].u_L10 * N.z +

      // band 2, values depend on multiple axes, higher frequency details
      C1 * lp.ambientProbes[i.x][i.y][i.z].u_L22 * (N.x * N.x - N.y * N.y) +
      C3 * lp.ambientProbes[i.x][i.y][i.z].u_L20 * N.z * N.z - C5 * lp.ambientProbes[i.x][i.y][i.z].u_L20 +
      2.0 * C1 * lp.ambientProbes[i.x][i.y][i.z].u_L2m2 * N.x * N.y +
      2.0 * C1 * lp.ambientProbes[i.x][i.y][i.z].u_L21 * N.x * N.z +
      2.0 * C1 * lp.ambientProbes[i.x][i.y][i.z].u_L2m1 * N.y * N.z;
}


void main() {

    
  ivec3 l_probeId;
  float mov = 0;//((sin(time)*0.5+0.5) * 10);
  float mov2 = 0;//((cos(time)*0.5+0.5) * 10);

  vec3 SPHFragPos = fs_in.wFragPos + vec3(35, 0, 32);

  vec3 gridSize = vec3(10, 5, 10);

  //l_probeId.x = //(myround(SPHFragPos.x, 10) + 35);
  //l_probeId.y = //(myround(SPHFragPos.y, 5));
  //l_probeId.z = //(myround(SPHFragPos.z, 10) + 35);
  l_probeId = ivec3(SPHFragPos) /*+ ivec3(34, 0, 34)*/;

    vec3 l_probe_pos = l_probeId;

    l_probeId /= ivec3(10, 5, 10);

    l_probeId.x = clamp(l_probeId.x, 0, 7); 
    l_probeId.y = clamp(l_probeId.y, 0, 7); 
    l_probeId.z = clamp(l_probeId.z, 0, 7); 

    int index = int(l_probeId.x) + (int(l_probeId.y)*7) + (int(l_probeId.z)*WIDTH*DEPTH);
    //int index = int(l_probeId.x) + WIDTH * (int(l_probeId.y) + DEPTH * int(l_probeId.z));


    //index = clamp(index, 0, 7*7*7);
    vec3 c000 = sphericalHarmonics(normalize(fs_in.Normal), l_probeId);

    FragColor = vec4(c000, 1.0);
    BrightFragColor = vec4(vec3(0.0), 1.0);
}