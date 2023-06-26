# OpenGL Showcase

## Table of Contents
- ### [Extra features](#extra-features-1).
- ### [Procedural textures (Perlin Noise)](#procedural-textures-perlin-noise-1)
- ### [Point lights](#point-lights-1)
- ### [Normal map](#normal-map-1)
- ### [Shadow map](#shadow-map-1)
- ### [Skybox](#skybox-1)
- ### [Phong Light System](#phong-light-system-1)

## Extra features
- Procedural textures (Perlin Noise).
- Point lights.
- Normal map.
- Shadow map.
- Light map.
- Skybox.

## Procedural textures (Perlin Noise)
The Sun object showcases a procedural texture obtained by the application of a 3D slice of a 4D Perlin Noise.  
The texture, and thus the base color for each pixel, is calculated by a Perlin Noise function with respect to a time parameter to change over time and give it an ever-changing appearance.  
Procedural textures find their roots in texture sculpting, and are used to overcome the memory challenges that the latter would come with.  
In particular, Perlin Noise is the sum of different noise functions with different parameters to combine bigger yet more regular noises together and give a better and more natural look.

## Point lights
The Sun and the Moon objects both contain a different point light.  
A point light is nothing more than a light source in the scene with a well defined origin and irradiating its light all over the scene in equal measure over distance.  
It differs from a directional light because a directional light has all its rays parallel and does not have an actual origin in the scene.  
It differs from a spotlight in such a point light rays go into all directions, unlike a spotlight whose rays have bounded by a light cone.  
It have some properties, namely:
- color: light color. The Sun has a yellowish light, while the Moon has a blueish one.
- position: light source position.
- diffusive: diffusive component, how much intensity the light has when diffusing from a surface.
- specular: specular component, how much intensity the light has when reflecting from a surface.
- constant: attenuation coefficient, used to decrease the amount of light over distance. It is constant and as such it is actually not related to the distance.
- linear: attenuation coefficient, used to decrease the amount of light over distance. It scales proportionally to the distance.
- quadratic: attenuation coefficient, used to decrease the amount of light over distance. It is scales quadratically to the distance.

While in the scene, each point light will illuminate all the scene it can see (depends on light far plane) in a sphere, with the amount of light decreasing with the distance from the point light source.  
Without the [shadow map](#shadow-map-1), each point light would illuminate the scene regardless of the objects in front of it.  
Many light properties come from the [Phong Light System](#phong-light-system-1).

## Normal map
Normal maps are essentialy textures to change the computed normals of an object, to add more variety to the normals of the object it is applied to.  
They are stored as RGB images, where each color component corresponds to a change in the x, y or z normal component.  
This has the effect of changing how the object surfaces interact with light, allowing for a more complex look without any extra geometry complexity.  
In this showcase, both the Sun and the Moon objects have a normal map applied, the Moon being the one for which it is most noticeable. It is easy to see how the normal map makes the Moon look more knurled.

## Shadow map
To create realistic shadows, a shadow map for each light source is applied.  
A shadow map is basically a render of the scene as seen from the light source point of view, creating a depth map and allowing to cast shadows from closer fragments on further ones.  
However, despite this working well for a directional light whose rays are parallel and thus there is only one directions, for a point light whose rays go in all direction a simple 2D map won't suffice. To overcome this, a cubemap (for each point light) is used instead:  
[INSERT IMAGE]  
Each vertex of each triangle is processed by the `shadow.geom` geometry shader and projected onto each of the 6 faces of the cubemap. Next, the `shadow.frag` fragment shader calculates the new fragment depths as the normalized distance from the light source.  
When this new data is passed to the `common.frag` fragment shader, the method `calculateShadow` calculates whether the current fragment is in shadow or exposed to light. This value is used as a multiplier for the light (more on how the light is calculated in the [Phong Light System](#phong-light-system-1) section) such that when the fragment is in shadow, the light is nullified, and when it's exposed, the light is left as is.  
This works quite well, but there is a very important detail to not overlook: since point lights are pointlike, to make it look like spherical objects are emitting it, the light source needs to be positioned inside of them. This however, would make the light trapped inside the object that was supposed to emit it, as each ray casting from the light is blocked by the spherical object around it. The solution to this issue is as simple as not drawing shadows for the objects that are supposed to be emitting light, along with setting the emissive light component of the object material to some value greater than 0, and most likely also setting the material color to whichever light color there is.  
Finally, talking about imaginary rays is nice and all, but how is this actually implemented? Simply put, the shadow map has a resolution in terms of the number of pixels used to create the dynamic shadow texture. The fewer pixels there are, the more the shadows are blocky and approximated. The more pixels, the smoother the shadows.

## Skybox
To give the illusion of a bigger and more filled space, a skybox is used.  
A skybox is just a cubemap applied to the camera view, such that in any direction the camera looks at it will always see the cubemap texture when no objects are in front of it.

## Phong Light System
The Phong Light System is a good approximation of local illumination.  
It consists of one main equation:
[INSERT IMAGE]
Where all the `k`s are properties of the material, while all the `I`s are properties of the light (apart from the first which is a constant).  
The first term is the emissive light component, the second one is the ambiental light component. Both of this are indipendent of any light source.  
The terms in the summation are instead dependent on the specific light source, and depend also on certain angles: the first one is the diffusive component and indicates how much of the incident light is scattered by the surface, depending on the angle between the (normal of the) surface and the incident light; the second one is the specular component and indicates how much of the incident light is reflected, depending on the angle between the reflected light direction and the camera view direction.  
Finally the attenuation function is defined as:
[INSERT IMAGE]
Decreasing the light intensity over distance based on the intensity reduction constants, properties of the light.

This whole equation is applied for each channel color.  
In the codebase, the `common.frag` fragment shader handles applying this math.

<br/>

#### *2023 Riccardo Chimisso 866009*
