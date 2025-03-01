# COMP3015 Coursework 1
An OpenGL project utilizing a custom shading model implemented in GLSL, with custom lighting set up, showcasing a textured based technique.

This project has been written and tested using Visual Studio 2022 on a Windows 10 machine.

Custom Revolver Model: https://sketchfab.com/3d-models/38-special-revolver-3246c31648204534a2c6d671fe38452e

The main files to look at are the following:
- scenebasic_uniform.h + scenebasic_uniform.cpp
- basic_uniform.vert + basic_uniform.frag
- plane.vert + plane.frag
- skybox.vert + skybox.frag

## YouTube Video Walkthrough
YouTube Video: [insert here]

## Basic Lighting - Blinnphong in Fragment Shader
Seen in basic_uniform.frag as the blinnphongSpot function, which is adapted for spotlights.
![Blinnphong](./images/blinnphong_fragment.png)

## Basic Texture Sampling
Seen in basic_uniform.frag as part of the blinnphongSpot function, using the sampled texture colour instead of the material colour.

![Texture Sampling](./images/texture_sampling.png)

## Lighting subtechniques - Multilight + Spotlight
The scene uses multiple spotlights to provide lighting.

![SpotLightInfo Uniform Struct](./images/lighting1.png)
![Setting Spotlight uniforms](./images/lighting2.png)
![Multilight For Loop](./images/lighting3.png)
![Spotlight Angle Check](./images/lighting4.png)

## Texturing subtechniques
![Texture Loading](./images/texture_loading.png)

### Alpha Discard
An alpha map is used to create a disintegration effect.

![Alpha Map](./media/texture/dots_alpha.png)
![Alpha Discard](./images/alpha_discard.png)

### Normal Mapping
A normal map is used to calculate normals for lighting.

![Normal Map](./images/normal_map.png)

## Skybox
A skybox is loaded in as a cubemap. The view is set at (0,0,0) before rendering to make the skybox seem infinite.

![Skybox Rendering](./images/skybox1.png)
![Skybox Vertex Shader](./images/skybox2.png)
![Skybox Fragment Shader](./images/skybox3.png)

## Light Animation
In the update function, an angle is updated every frame. This angle is used to rotate the lights.

![Angle Update](./images/animation1.png)
![Spotlight Position Uniforms](./images/animation2.png)

## Image Processing Techniques
Multiple image processing techniques have been combined, primarily to create the Bloom effect.

![Uniform Setup](./images/image_proc_tech_uniforms.png)

### HDR + Tone Mapping
Applied in pass 5.

![HDR + Tone Mapping](./images/image_proc_tech1.png)

### Bloom
Luminance checks done in pass 2, and blurring is applied additively in pass 5 with HDR.

![Luminance Checks](./images/image_proc_tech1.5.png)
![Additive Blurring](./images/image_proc_tech2.png)

### Gaussian Blur
Applied in passes 3 and 4 for y and x directions respectively.

![Gaussian Blur](./images/image_proc_tech3.png)

### Gamma Correction
Applied to the result of pass 5 for the final image.

![Gamma Correction](./images/image_proc_tech4.png)

## Keyboard Scene/Animation Controls
Keyboard input is checked in update function:
Move Camera Around - WASD
Toggle Alpha Map - 1
Toggle RGB/White Lights - 2
Toggle Bloom - 3

![Keyboard Inputs](./images/keyboard1.png)
![Keyboard Inputs](./images/keyboard2.png)

## Mouse Scene/Animation Controls
Mouse input is checked in update function:
Rotate Camera - Move Mouse Around

![Mouse Input](./images/mouse.png)