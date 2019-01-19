# a3 Ray Tracer

## Notes to developers
* Every file should include "Common/Core.h"
* v2, v3, v4 are treated as default data types, and are not initialized to 0
* Replaced math functions such as `Sinf`, `Cosf` etc are in global namespace and not in `a3`
* Math functions for v2, v3 and v4 are made in global namespace and not in `a3`
* Similarly `m4x4` and `quat` are in global namespace and not in `a3`. To add to the fact all
 mathematics structs/classes/functions are in global namespace. This is intensional.
* Identifiers that are internal to a system/code/file should begin with `a3_` prefix and should not be present in `a3` namespace.
 For example:
 ```cpp
 void a3_Log();
 ```
* All other structs/classes/function/enums are and should be inside `a3` namespace unless there's a good reason not to.
* All preprocessor definations that can be used in any part of code must begin with **A3**
 following with the defination name, should contain no space(' ') and must be all capitals.
 For example: 
 ```cpp
 #define A3NULL 0
 #define A3DEBUG
 ```
* Preprocessor that are only internally used by the single part of the code and made with no intension
 of using it outside that scope should also begin with **A3** but every word should be seperated by
 a underscore('_') and should all be capitals. For example:
 ```cpp
 #define A3_RENDER_MAX_COUNT 1000
 #define A3_WINDOW_CLASS_NAME "WindowClassName"
 ```
* All macros should begin with `a3` and should be camel case with first letter being capital.
 For example:
 ```cpp
 #define a3Bit(n) (1 << (n))
 ```
* Structs, classes, enums names should be all lower case with seperating by underscore('_').
  For example:
  ```cpp
  struct v2;
  struct font_renderer;
  ```
* Private members should have prefix `m_` and should be camel case with first letter being capital.
* All functions should be "CamelCased" but if internal should have prefix `a3_`
* All internal static identifiers should have prefix `s_` and globals should have prefix `g_`
* If globals identifiers that are to be used by other parts of the system/code should be "CamelCased"
* **External** libraries do not have to follow these conventions but should be wrapped in our code before usage
* All external libraries must be made to use our memory allocators and our libraries and not standard libraries
