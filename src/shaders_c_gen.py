#!/usr/bin/env python3

class Shader:
    def __init__(self, name, vs_path, fs_path, attributes, uniforms, subroutines):
        self.name       = name
        self.vs_path    = vs_path
        self.fs_path    = fs_path
        self.attributes = attributes
        self.uniforms   = uniforms
        self.subroutines= subroutines

    def header_chunks(self):
        yield "   struct {\n"
        yield "      GLuint program;\n"
        for attribute in self.attributes:
            yield f"      GLuint {attribute};\n"
        for uniform in self.uniforms:
            yield f"      GLuint {uniform};\n"
        for subroutine, routines in self.subroutines.items():
            yield "      struct {\n"
            yield "         GLuint uniform;\n"
            for r in routines:
                yield f"         GLuint {r};\n"
            yield f"      }} {subroutine};\n"
        yield f"   }} {self.name};\n"

    def source_chunks(self):
        yield f"   shaders.{self.name}.program = gl_program_vert_frag(\"{self.vs_path}\", \"{self.fs_path}\");\n"
        for attribute in self.attributes:
            yield f"   shaders.{self.name}.{attribute} = glGetAttribLocation(shaders.{self.name}.program, \"{attribute}\");\n"
        for uniform in self.uniforms:
            yield f"   shaders.{self.name}.{uniform} = glGetUniformLocation(shaders.{self.name}.program, \"{uniform}\");\n"
        if len(self.subroutines) > 0:
            yield "   if (gl_has( OPENGL_SUBROUTINES )) {\n"
            for subroutine, routines in self.subroutines.items():
                yield f"      shaders.{self.name}.{subroutine}.uniform = glGetSubroutineUniformLocation( shaders.{self.name}.program, GL_FRAGMENT_SHADER, \"{subroutine}\" );\n"
                for r in routines:
                    yield f"      shaders.{self.name}.{subroutine}.{r} = glGetSubroutineIndex( shaders.{self.name}.program, GL_FRAGMENT_SHADER, \"{r}\" );\n"
            yield "   }\n"


class SimpleShader(Shader):
    def __init__(self, name, fs_path):
        super().__init__( name=name, vs_path="project_pos.vert", fs_path=fs_path, attributes=["vertex"], uniforms=["projection","color","dimensions","dt","paramf","parami","paramv"], subroutines={} )
    def header_chunks(self):
        yield f"   SimpleShader {self.name};\n"


SHADERS = [
   Shader(
      name = "circle_partial",
      vs_path = "circle.vert",
      fs_path = "circle_partial.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "color", "radius", "angle1", "angle2"],
      subroutines = {},
   ),
   Shader(
      name = "solid",
      vs_path = "project.vert",
      fs_path = "solid.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "color"],
      subroutines = {},
   ),
   Shader(
      name = "trail",
      vs_path = "project_pos.vert",
      fs_path = "trail.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "c1", "c2", "t1", "t2", "dt", "pos1", "pos2", "r", "nebu_col" ],
      subroutines = {
        "trail_func" : [
            "trail_default",
            "trail_pulse",
            "trail_wave",
            "trail_flame",
            "trail_nebula",
            "trail_arc",
            "trail_bubbles",
        ]
      }
   ),
   Shader(
      name = "smooth",
      vs_path = "smooth.vert",
      fs_path = "smooth.frag",
      attributes = ["vertex", "vertex_color"],
      uniforms = ["projection"],
      subroutines = {},
   ),
   Shader(
      name = "texture",
      vs_path = "texture.vert",
      fs_path = "texture.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "color", "tex_mat"],
      subroutines = {},
   ),
   Shader(
      name = "texture_interpolate",
      vs_path = "texture.vert",
      fs_path = "texture_interpolate.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "color", "tex_mat", "sampler1", "sampler2", "inter"],
      subroutines = {},
   ),
   Shader(
      name = "nebula",
      vs_path = "nebula.vert",
      fs_path = "nebula_overlay.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "hue", "brightness", "horizon", "eddy_scale", "time"],
      subroutines = {},
   ),
   Shader(
      name = "nebula_background",
      vs_path = "nebula.vert",
      fs_path = "nebula_background.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "hue", "brightness", "eddy_scale", "time"],
      subroutines = {},
   ),
   Shader(
      name = "nebula_map",
      vs_path = "nebula_map.vert",
      fs_path = "nebula_map.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "hue", "eddy_scale", "time", "globalpos", "alpha"],
      subroutines = {},
   ),
   Shader(
      name = "stars",
      vs_path = "stars.vert",
      fs_path = "stars.frag",
      attributes = ["vertex", "brightness", "relspeed", "color"],
      uniforms = ["projection", "star_xy", "wh", "xy", "scale"],
      subroutines = {},
   ),
   Shader(
      name = "font",
      vs_path = "font.vert",
      fs_path = "font.frag",
      attributes = ["vertex", "tex_coord"],
      uniforms = ["projection", "m", "color", "outline_color"],
      subroutines = {},
   ),
   Shader(
      name = "beam",
      vs_path = "project_pos.vert",
      fs_path = "beam.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "color", "dt", "r", "dimensions" ],
      subroutines = {
        "beam_func" : [
            "beam_default",
            "beam_wave",
            "beam_arc",
            "beam_helix",
            "beam_organic",
            "beam_unstable",
            "beam_fuzzy",
        ]
      }
   ),
   Shader(
      name = "jump",
      vs_path = "project_pos.vert",
      fs_path = "jump.frag",
      attributes = ["vertex"],
      uniforms = ["projection", "progress", "direction", "dimensions"],
      subroutines = {
        "jump_func" : [
            "jump_default",
            "jump_nebula",
            "jump_organic",
            "jump_circular",
            "jump_wind",
        ]
      }
   ),
   Shader(
      name = "material",
      vs_path = "material.vert",
      fs_path = "material.frag",
      attributes = ["vertex", "vertex_normal", "vertex_tex"],
      uniforms = ["projection", "model", "map_Kd", "map_Ks", "map_Ke", "map_Bump", "Ns", "Ka", "Kd", "Ks", "Ke", "Ni", "d", "bm"],
      subroutines = {},
   ),
   Shader(
      name = "colorblind",
      vs_path = "postprocess.vert",
      fs_path = "colorblind.frag",
      attributes = ["VertexPosition"],
      uniforms = ["ClipSpaceFromLocal", "MainTex"],
      subroutines = {},
   ),
   Shader(
      name = "shake",
      vs_path = "postprocess.vert",
      fs_path = "shake.frag",
      attributes = ["VertexPosition"],
      uniforms = ["ClipSpaceFromLocal", "MainTex", "shake_pos", "shake_vel", "shake_force"],
      subroutines = {},
   ),
   Shader(
      name = "gamma_correction",
      vs_path = "postprocess.vert",
      fs_path = "gamma_correction.frag",
      attributes = ["VertexPosition"],
      uniforms = ["ClipSpaceFromLocal", "MainTex", "gamma"],
      subroutines = {},
   ),
   SimpleShader(
      name = "status",
      fs_path = "status.frag",
   ),
   SimpleShader(
      name = "factiondisk",
      fs_path = "factiondisk.frag",
   ),
   SimpleShader(
      name = "planetmarker",
      fs_path = "planetmarker.frag",
   ),
   SimpleShader(
      name = "jumpmarker",
      fs_path = "jumpmarker.frag",
   ),
   SimpleShader(
      name = "pilotmarker",
      fs_path = "pilotmarker.frag",
   ),
   SimpleShader(
      name = "playermarker",
      fs_path = "playermarker.frag",
   ),
   SimpleShader(
      name = "blinkmarker",
      fs_path = "blinkmarker.frag",
   ),
   SimpleShader(
      name = "sysmarker",
      fs_path = "sysmarker.frag",
   ),
   SimpleShader(
      name = "asteroidmarker",
      fs_path = "asteroidmarker.frag",
   ),
   SimpleShader(
      name = "targetship",
      fs_path = "targetship.frag",
   ),
   SimpleShader(
      name = "targetplanet",
      fs_path = "targetplanet.frag",
   ),
   SimpleShader(
      name = "sdfsolid",
      fs_path = "sdfsolid.frag",
   ),
   SimpleShader(
      name = "circle",
      fs_path = "circle.frag",
   ),
   SimpleShader(
      name = "crosshairs",
      fs_path = "crosshairs.frag",
   ),
   SimpleShader(
      name = "hilight",
      fs_path = "hilight.frag",
   ),
   SimpleShader(
      name = "hilight_pos",
      fs_path = "hilight_pos.frag",
   ),
   SimpleShader(
      name = "hilight_circle",
      fs_path = "hilight_circle.frag",
   ),
   SimpleShader(
      name = "progressbar",
      fs_path = "progressbar.frag",
   ),
]

def header_chunks():
    yield f"/* FILE GENERATED BY {__file__} */"

def generate_h_file():
    yield from header_chunks()

    yield """
#ifndef SHADER_GEN_C_H
#define SHADER_GEN_C_H
#include "glad.h"

typedef struct SimpleShader_ {
    GLuint program;
    GLuint vertex;
    GLuint projection;
    GLuint color;
    GLuint dimensions;
    GLuint dt;
    GLuint parami;
    GLuint paramf;
    GLuint paramv;
} SimpleShader;

typedef struct Shaders_ {
"""

    for shader in SHADERS:
        yield from shader.header_chunks()

    yield """} Shaders;

extern Shaders shaders;

void shaders_load (void);
void shaders_unload (void);

#endif /* SHADERS_GEN_C_H */"""

def generate_c_file():
    yield from header_chunks()

    yield """
#include <string.h>
#include "shaders.gen.h"
#include "opengl_shader.h"

Shaders shaders;

void shaders_load (void) {
"""
    for i, shader in enumerate(SHADERS):
        yield from shader.source_chunks()
        if i != len(SHADERS) - 1:
            yield "\n"
    yield """}

void shaders_unload (void) {
"""
    for shader in SHADERS:
        yield f"   glDeleteProgram(shaders.{shader.name}.program);\n"

    yield """   memset(&shaders, 0, sizeof(shaders));
}"""

with open("shaders.gen.h", "w") as shaders_gen_h:
    shaders_gen_h.writelines(generate_h_file())

with open("shaders.gen.c", "w") as shaders_gen_c:
    shaders_gen_c.writelines(generate_c_file())
