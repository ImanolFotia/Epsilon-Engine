# API Mappings

The following is a list of DX11 <=> OpenGL concepts mapping
The list was taken from [this](https://computergraphics.stackexchange.com/questions/4422/directx-openglvulkan-concepts-mapping-chart) StackExchange thread, answered by Nathan Reed

## API Basics

| D3D11                        | OpenGL 4.x                                               |
| ---------------------------- | -------------------------------------------------------- |
| device                       | context                                                  |
| immediate context            | (implicit; no specific name)                             |
| deferred context             | (no cross-vendor equivalent, but see GL_NV_command_list) |
| swap chain                   | (implicit; no specific name)                             |
| (no cross-vendor equivalent) | extensions                                               |
| debug layer; info queue      | GL_KHR_debug extension                                   |

## Shaders

| D3D11                                       |                                                                                  | OpenGL 4.x                       |
| :------------------------------------------ | :------------------------------------------------------------------------------- | :------------------------------- |
| pixel shader                                |                                                                                  | fragment shader                  |
| hull shader                                 |                                                                                  | tessellation control shader      |
| domain shader                               |                                                                                  | tessellation evaluation shader   |
|                                             | (vertex shader, geometry shader, and compute shader are called the same in both) |                                  |
| registers                                   |                                                                                  | binding points                   |
| semantics                                   |                                                                                  | interface layouts                |
| SV_Foo semantics                            |                                                                                  | gl_Foo builtin variables         |
| class linkage                               |                                                                                  | subroutines                      |
| (no equivalent)                             |                                                                                  | program objects; program linking |
| (D3D11's normal behavior; no specific name) |                                                                                  | separate shader objects          |

## Geometry and Drawing

| D3D11           |                                                             | OpenGL 4.x                                          |
| :-------------- | :---------------------------------------------------------: | :-------------------------------------------------- |
| vertex buffer   |                                                             | vertex attribute array buffer; vertex buffer object |
| index buffer    |                                                             | element array buffer                                |
| input layout    |                                                             | vertex array object (sort of)                       |
| Draw            |                                                             | glDrawArrays                                        |
| DrawIndexed     |                                                             | glDrawElements                                      |
|                 | (instancing and indirect draw are called similarly in both) |
| (no equivalent) |                                                             | multi-draw, e.g. glMultiDrawElements​               |
| stream-out      |                                                             | transform feedback                                  |
| DrawAuto        |                                                             | glDrawTransformFeedback​                            |
| predication     |                                                             | conditional rendering                               |
| (no equivalent) |                                                             | sync objects                                        |

## Buffers and Textures

| D3D11                                       |     | OpenGL 4.x                                  |
| :------------------------------------------ | :-- | :------------------------------------------ |
| constant buffer                             |     | uniform buffer object                       |
| typed buffer                                |     | texture buffer                              |
| structured buffer                           |     | (no specific name; subset of SSBO features) |
| UAV buffer; RWBuffer                        |     | SSBO (shader storage buffer object)         |
| UAV texture; RWTexture                      |     | image load/store                            |
| shader resource view                        |     | texture view                                |
| sampler state                               |     | sampler object                              |
| interlocked operations                      |     | atomic operations                           |
| append/consume buffer                       |     | SSBO + atomic counter                       |
| discard buffer/texture                      |     | invalidate buffer/texture                   |
| (no equivalent)                             |     | persistent mapping                          |
| (D3D11's normal behavior; no specific name) |     | immutable storage                           |
| (implicitly inserted by the API)            |     | glMemoryBarrier; glTextureBarrier           |

## Render Targets

| D3D11                   |     | OpenGL 4.x                                       |
| :---------------------- | :-- | :----------------------------------------------- |
| (no equivalent)         |     | framebuffer object                               |
| render target view      |     | framebuffer color attachment                     |
| depth-stencil view      |     | framebuffer depth-stencil attachment             |
| multisample resolve     |     | blit multisampled buffer to non-multisampled one |
| multiple render targets |     | multiple color attachments                       |
| render target array     |     | layered image                                    |
| (no equivalent)         |     | renderbuffer                                     |

## Queries

| D3D11                     |     | OpenGL 4.x                                                        |
| :------------------------ | :-- | :---------------------------------------------------------------- |
| timestamp query           |     | timer query                                                       |
| timestamp-disjoint query  |     | (no equivalent)                                                   |
| (no equivalent)           |     | time-elapsed query                                                |
| occlusion query           |     | samples-passed query                                              |
| occlusion predicate query |     | any-samples-passed query                                          |
| pipeline statistics query |     | (no equivalent in core, but see GL_ARB_pipeline_statistics_query) |
| SO statistics query       |     | primitives-generated/-written queries                             |
| (no equivalent)           |     | query buffer object                                               |

## Compute Shaders

| D3D11                 |     | OpenGL 4.x                          |
| :-------------------- | :-- | :---------------------------------- |
| thread                |     | invocation                          |
| thread group          |     | work group                          |
| thread group size     |     | local size                          |
| threadgroup variable  |     | shared variable                     |
| group sync            |     | "plain" barrier                     |
| group memory barrier  |     | shared memory barrier               |
| device memory barrier |     | atomic+buffer+image memory barriers |
| all memory barrier    |     | group memory barrier                |
