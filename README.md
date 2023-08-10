# Pigeon Engine
A game engine for pc (DirectX 11).

## Third Party Lib
| Name |  Description | URL |
| ---- | ---- | ---- |
| assimp | Open Asset Import Library is a library to load various 3d file formats into a shared, in-memory format. | [assimp](https://github.com/assimp/assimp) |
| imgui | Dear ImGui is a bloat-free graphical user interface library. | [imgui](https://github.com/ocornut/imgui) |
| rapidjson | A fast JSON parser/generator for C++ with both SAX/DOM style API. | [rapidjson](https://github.com/Tencent/rapidjson/) |
| JoltPhysics | A multi core friendly rigid body physics and collision detection library. | [JoltPhysics](https://github.com/jrouwe/JoltPhysics) |

## Type Name Rule
| Example Type Name |  Name Type Rule |
| ---- | ---- |
| _TClass_ | Naming conventions for template classes **START** with a "T" |
| _EnumType_ | Naming conventions for enum type **END** with a "Type" |
| _EClass_ _EStruct_ | Naming conventions for general classes in engines **START** with a "E" |
| _PClass_ _PStruct_ | Naming conventions for game object classes **START** with a "P" |
| _RClass_ _RStruct_ | Naming conventions for render pipeline classes **START** with a "R" |
| _FClass_ _FStruct_ | Naming conventions for physics classes **START** with a "F" |

## Engine Define
| Example Define Name |  Define Rule |
| ---- | ---- |
| _DEBUG_MODE | This mode will run with **complete** log and **complete** other profiler options. |
| _DEVELOP_MODE | This mode will run with **complete** log and **selectional** other profiler options. |
| _TEST_MODE | This mode will run with **selectional** log and **few** other profiler options. |
| _SHIPPING_MODE | This mode will run with **few** log and **NO** other profiler options. |
| _EDITOR_ONLY | This define makes a distinction between **Editor** and **Runtime**. |

## Engine System

### Actor System
TODO

### Physic System
TODO

### Render System
TODO
#### Material System
TODO
#### Render Features
TODO

## NOTE
Name of skeleton's bone can not large than 512 bytes(char[512]), because we use char[512] for serialization / deserialization
For now we can not import skeleton mesh that effect bone num is over 16
