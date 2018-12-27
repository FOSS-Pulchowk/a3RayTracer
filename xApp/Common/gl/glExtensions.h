// SOURCE: https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
// COPIED DATE: 12/27/2018

/*
Name
WGL_ARB_pixel_format

Name Strings
WGL_ARB_pixel_format

Contact
Pat Brown, NVIDIA(pbrown 'at' nvidia.com)
Paula Womack

Notice
Copyright(c) 2000 - 2013 The Khronos Group Inc.Copyright terms at
http ://www.khronos.org/registry/speccopyright.html

Status
Complete.Approved by ARB on 3 / 15 / 2000.

Version
Last Modified Date : March 25, 2009
Author Revision : 1.3

Number
ARB Extension #9

Dependencies
WGL_ARB_extensions_string is required.

Overview
This extension adds functions to query pixel format attributes and
to choose from the list of supported pixel formats.

These functions treat pixel formats as opaque types : attributes are
specified by name rather than by accessing them directly as fields
in a structure.Thus the list of attributes can be easily extended.

Attribute names are defined which correspond to all of the values in
the PIXELFORMATDESCRIPTOR and LAYERPLANEDESCRIPTOR data structures.
Additionally this interface allows pixel formats to be supported
which have attributes that cannot be represented using the standard
pixel format functions, i.e.DescribePixelFormat,
DescribeLayerPlane, ChoosePixelFormat, SetPixelFormat, and
GetPixelFormat.

IP Status

No issues.

Issues and Notes

1. No provision is made to support extended pixel format attributes in
metafiles.
2. Should the transparent value pixel format attribute have separate red,
green and blue values ? Yes.
3. What data type should the transparent value be ? This is no longer an
issue since the transparent value is no longer a packed pixel value(it
																	has separate r, g, b, a and index values).
	4. Should we add DONT_CARE values for some of the pixel format attributes ?
	No we should just ignore attributes that aren't specified in the list
	passed to wglChoosePixelFormatARB.
	5. Should wglGetPixelFormatAttrib*vARB ignore the <iLayerPlane> parameter
	when the attribute specified only applies to the main planes(e.g.,
																 when the attribute is set to WGL_NUMBER_OVERLAYS) or should it require
	<iLayerPlane> to be set to zero ? It will just ignore the parameter.
	This allows these attributes to be queried at the same time as
	attributes of the overlay planes.
	6. Should wglGetPixelFormatAttribivARB convert floating point values to
	fixed point ? No, wglChoosePixelFormatARB needs a way to accept floating
	point values.pfAttribFList accomplishes this.
	7. Should wglChoosePixelFormatARB take an <iLayerPlane> parameter ?
	Typically <iLayerPlane> would be set to zero and a pixel format would
	be selected based on the attributes of the main plane, so there is no
	<iLayerPlane> parameter.This should be OK; applications won't
	typically select a pixel format on the basis of overlay attributes.
	They can always call wglGetPixelFormatAttrib*vARB to get a pixel format
	that has the desired overlay values.
	8. Application programmers must check to see if a particular extension is
	supported before using any pixel format attributes associated with the
	extension.For example, if WGL_ARB_pbuffer is not supported then it is
	an error to specify WGL_DRAW_TO_PBUFFER_ARB in the attribute list to
	wglGetPixelFormatAttrib*vARB or wglChoosePixelFormatARB.
	9. Should WGLChoosePixelFormatARB consider pixel formats at other display
	depths ? It would be useful to have an argument to
	WGLChoosePixelFormatARB indicating what display depth should be used.
	However, there is no good way to implement this in the ICD since pixel
	format handles are sequential indices and the pixel format for index n
	differs depending on the display mode.
	10. Should we allow non - displayable pixel formats for pbuffers ? Yes,
	although many(most ? ) implementations will use displayable pixel
	formats for pbuffers, this is a useful feature and the spec should
	allow for it.
	11. Should we create all new calls for pixel formats, specifically should
	we introduce SetPixelFormatARB ? No, this doesn't offer any value over
	the existing SetPixelFormat call.
	12. Should we add support for triple buffering ? No, triple buffering needs
	to be covered by a separate extension.

	New Procedures and Functions

	BOOL wglGetPixelFormatAttribivARB(HDC hdc,
									  int iPixelFormat,
									  int iLayerPlane,
									  UINT nAttributes,
									  const int *piAttributes,
									  int *piValues);

BOOL wglGetPixelFormatAttribfvARB(HDC hdc,
								  int iPixelFormat,
								  int iLayerPlane,
								  UINT nAttributes,
								  const int *piAttributes,
								  FLOAT *pfValues);

BOOL wglChoosePixelFormatARB(HDC hdc,
							 const int *piAttribIList,
							 const FLOAT *pfAttribFList,
							 UINT nMaxFormats,
							 int *piFormats,
							 UINT *nNumFormats);

New Tokens

Accepted in the <piAttributes> parameter array of
wglGetPixelFormatAttribivARB, and wglGetPixelFormatAttribfvARB, and
as a type in the <piAttribIList> and <pfAttribFList> parameter
arrays of wglChoosePixelFormatARB :
*/
#define WGL_NUMBER_PIXEL_FORMATS_ARB            0x2000
#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_DRAW_TO_BITMAP_ARB                  0x2002
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_NEED_PALETTE_ARB                    0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB             0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB              0x2006
#define WGL_SWAP_METHOD_ARB                     0x2007
#define WGL_NUMBER_OVERLAYS_ARB                 0x2008
#define WGL_NUMBER_UNDERLAYS_ARB                0x2009
#define WGL_TRANSPARENT_ARB                     0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB           0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB         0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB          0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB         0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB         0x203B
#define WGL_SHARE_DEPTH_ARB                     0x200C
#define WGL_SHARE_STENCIL_ARB                   0x200D
#define WGL_SHARE_ACCUM_ARB                     0x200E
#define WGL_SUPPORT_GDI_ARB                     0x200F
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_STEREO_ARB                          0x2012
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_COLOR_BITS_ARB                      0x2014
#define WGL_RED_BITS_ARB                        0x2015
#define WGL_RED_SHIFT_ARB                       0x2016
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_GREEN_SHIFT_ARB                     0x2018
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_BLUE_SHIFT_ARB                      0x201A
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_ALPHA_SHIFT_ARB                     0x201C
#define WGL_ACCUM_BITS_ARB                      0x201D
#define WGL_ACCUM_RED_BITS_ARB                  0x201E
#define WGL_ACCUM_GREEN_BITS_ARB                0x201F
#define WGL_ACCUM_BLUE_BITS_ARB                 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB                0x2021
#define WGL_DEPTH_BITS_ARB                      0x2022
#define WGL_STENCIL_BITS_ARB                    0x2023
#define WGL_AUX_BUFFERS_ARB                     0x2024

/*
Accepted as a value in the <piAttribIList> and <pfAttribFList>
parameter arrays of wglChoosePixelFormatARB, and returned in the
<piValues> parameter array of wglGetPixelFormatAttribivARB, and the
<pfValues> parameter array of wglGetPixelFormatAttribfvARB :
*/

#define WGL_NO_ACCELERATION_ARB                 0x2025
#define WGL_GENERIC_ACCELERATION_ARB            0x2026
#define WGL_FULL_ACCELERATION_ARB               0x2027

#define WGL_SWAP_EXCHANGE_ARB                   0x2028
#define WGL_SWAP_COPY_ARB                       0x2029
#define WGL_SWAP_UNDEFINED_ARB                  0x202A

#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_TYPE_COLORINDEX_ARB                 0x202C

/*
Additions to Chapter 2 of the 1.2 Specification(OpenGL Operation)

None
Additions to Chapter 3 of the 1.2 Specification(Rasterization)

None
Additions to Chapter 4 of the 1.2 Specification(Per - Fragment Operations and
												the Frame buffer)

	None

	Additions to Chapter 5 of the 1.2 Specification(Special Functions)

	None

	Additions to Chapter 6 of the 1.2 Specification(State and State Requests)

	None

	Additions to the WGL Specification

	Pixel Formats

	WGL uses pixel format indices to refer to the pixel formats
	supported by a device.The standard pixel format functions
	DescribePixelFormat, DescribeLayerPlane, ChoosePixelFormat,
	SetPixelFormat, and GetPixelFormat specify pixel format attributes
	using the PIXELFORMATDESCRIPTOR and LAYERPLANEDESCRIPTOR data
	structures.

	An additional set of functions may be used to query and specify
	pixel format attributes by name.

	Querying Pixel Format Attributes

	The following two functions can be used to query pixel format
	attributes by specifying a list of attributes to be queried and
	providing a buffer in which to receive the results from the query.
	These functions can be used to query the attributes of both the main
	plane and layer planes of a given pixel format.

	BOOL wglGetPixelFormatAttribivARB(HDC hdc,
									  int iPixelFormat,
									  int iLayerPlane,
									  UINT nAttributes,
									  const int *piAttributes,
									  int *piValues);

<hdc> specifies the device context on which the pixel format is
supported.

<iPixelFormat> is an index that specifies the pixel format.The
pixel formats that a device context supports are identified by
positive one - based integer indexes.

<iLayerPlane> specifies which plane is being queried.Positive
values of <iLayerPlane> identify overlay planes, where 1 is the
first overlay plane over the main plane, 2 is the second overlay
plane over the first overlay plane, and so on.Negative values
identify underlay planes, where - 1 is the first underlay plane under
the main plane, -2 is the second underlay plane under the first
underlay plane and so on.Use zero for the main plane.

<nAttributes> number of attributes being queried.

<piAttributes> list containing an array of pixel format attribute
identifiers which specify the attributes to be queried.The
following values are accepted :

WGL_NUMBER_PIXEL_FORMATS_ARB
The number of pixel formats for the device context.The
<iLayerPlane> and <iPixelFormat> parameters are ignored if this
attribute is specified.

WGL_DRAW_TO_WINDOW_ARB
True if the pixel format can be used with a window.The
<iLayerPlane> parameter is ignored if this attribute is
specified.

WGL_DRAW_TO_BITMAP_ARB
True if the pixel format can be used with a memory bitmap.The
<iLayerPlane> parameter is ignored if this attribute is
specified.

WGL_ACCELERATION_ARB
Indicates whether the pixel format is supported by the driver.
If this is set to WGL_NO_ACCELERATION_ARB then only the software
renderer supports this pixel format; if this is set to
WGL_GENERIC_ACCELERATION_ARB then the pixel format is supported
by an MCD driver; if this is set to WGL_FULL_ACCELERATION_ARB
then the pixel format is supported by an ICD driver.

WGL_NEED_PALETTE_ARB
A logical palette is required to achieve the best results for
this pixel format.The <iLayerPlane> parameter is ignored if
this attribute is specified.

WGL_NEED_SYSTEM_PALETTE_ARB
The hardware supports one hardware palette in 256 - color mode
only.The <iLayerPlane> parameter is ignored if this attribute
is specified.

WGL_SWAP_LAYER_BUFFERS_ARB
True if the pixel format supports swapping layer planes
independently of the main planes.If the pixel format does not
support a back buffer then this is set to FALSE.The
<iLayerPlane> parameter is ignored if this attribute is
specified.

WGL_SWAP_METHOD_ARB
If the pixel format supports a back buffer, then this indicates
how they are swapped.If this attribute is set to
WGL_SWAP_EXCHANGE_ARB then swapping exchanges the front and back
buffer contents; if it is set to WGL_SWAP_COPY_ARB then swapping
copies the back buffer contents to the front buffer; if it is
set to WGL_SWAP_UNDEFINED_ARB then the back buffer contents are
copied to the front buffer but the back buffer contents are
undefined after the operation.If the pixel format does not
support a back buffer then this parameter is set to
WGL_SWAP_UNDEFINED_ARB.The <iLayerPlane> parameter is ignored
if this attribute is specified.

WGL_NUMBER_OVERLAYS_ARB
The number of overlay planes.The <iLayerPlane> parameter is
ignored if this attribute is specified.

WGL_NUMBER_UNDERLAYS_ARB
The number of underlay planes.The <iLayerPlane> parameter is
ignored if this attribute is specified.

WGL_TRANSPARENT_ARB
True if transparency is supported.

WGL_TRANSPARENT_RED_VALUE_ARB
Specifies the transparent red color value.Typically this value
is the same for all layer planes.This value is undefined if
transparency is not supported.

WGL_TRANSPARENT_GREEN_VALUE_ARB
Specifies the transparent green value.Typically this value is
the same for all layer planes.This value is undefined if
transparency is not supported.

WGL_TRANSPARENT_BLUE_VALUE_ARB
Specifies the transparent blue color value.Typically this value
is the same for all layer planes.This value is undefined if
transparency is not supported.

WGL_TRANSPARENT_ALPHA_VALUE_ARB
Specifies the transparent alpha value.This is reserved for
future use.

WGL_TRANSPARENT_INDEX_VALUE_ARB
Specifies the transparent color index value.Typically this
value is the same for all layer planes.This value is undefined
if transparency is not supported.

WGL_SHARE_DEPTH_ARB
True if the layer plane shares the depth buffer with the main
planes.If <iLayerPlane> is zero, this is always true.

WGL_SHARE_STENCIL_ARB
True if the layer plane shares the stencil buffer with the main
planes.If <iLayerPlane> is zero, this is always true.

WGL_SHARE_ACCUM_ARB
True if the layer plane shares the accumulation buffer with the
main planes.If <iLayerPlane> is zero, this is always true.

WGL_SUPPORT_GDI_ARB
True if GDI rendering is supported.

WGL_SUPPORT_OPENGL_ARB
True if OpenGL is supported.

WGL_DOUBLE_BUFFER_ARB
True if the color buffer has back / front pairs.

WGL_STEREO_ARB
True if the color buffer has left / right pairs.

WGL_PIXEL_TYPE_ARB
The type of pixel data.This can be set to WGL_TYPE_RGBA_ARB or
WGL_TYPE_COLORINDEX_ARB.

WGL_COLOR_BITS_ARB
The number of color bitplanes in each color buffer.For RGBA
pixel types, it is the size of the color buffer, excluding the
alpha bitplanes.For color - index pixels, it is the size of the
color index buffer.

WGL_RED_BITS_ARB
The number of red bitplanes in each RGBA color buffer.

WGL_RED_SHIFT_ARB
The shift count for red bitplanes in each RGBA color buffer.

WGL_GREEN_BITS_ARB
The number of green bitplanes in each RGBA color buffer.

WGL_GREEN_SHIFT_ARB
The shift count for green bitplanes in each RGBA color buffer.

WGL_BLUE_BITS_ARB
The number of blue bitplanes in each RGBA color buffer.

WGL_BLUE_SHIFT_ARB
The shift count for blue bitplanes in each RGBA color buffer.

WGL_ALPHA_BITS_ARB
The number of alpha bitplanes in each RGBA color buffer.

WGL_ALPHA_SHIFT_ARB
The shift count for alpha bitplanes in each RGBA color buffer.

WGL_ACCUM_BITS_ARB
The total number of bitplanes in the accumulation buffer.

WGL_ACCUM_RED_BITS_ARB
The number of red bitplanes in the accumulation buffer.

WGL_ACCUM_GREEN_BITS_ARB
The number of green bitplanes in the accumulation buffer.

WGL_ACCUM_BLUE_BITS_ARB
The number of blue bitplanes in the accumulation buffer.

WGL_ACCUM_ALPHA_BITS_ARB
The number of alpha bitplanes in the accumulation buffer.

WGL_DEPTH_BITS_ARB
The depth of the depth(z - axis) buffer.

WGL_STENCIL_BITS_ARB
The depth of the stencil buffer.

WGL_AUX_BUFFERS_ARB
The number of auxiliary buffers.

<piValues> points to a buffer into which the results of the query
will be placed.Floating point attribute values are rounded to the
nearest integer value.The caller must allocate this array and it
must have at least <nattributes> entries.

If the function succeeds, the return value is TRUE.If the function
fails, the return value is FALSE.To get extended error information,
call GetLastError.

An error is generated if <piAttributes> contains an invalid
attribute, if <iPixelFormat> is not a positive integer or is larger
than the number of pixel formats, if <iLayerPlane> doesn't refer to
an existing layer plane, or if <hdc> is invalid.

If FALSE is returned, the contents of <piValues> are undefined.

BOOL wglGetPixelFormatAttribfvARB(HDC hdc,
								  int iPixelFormat,
								  int iLayerPlane,
								  UINT nAttributes,
								  const int *piAttributes,
								  FLOAT *pfValues);

<hdc> specifies the device context on which the pixel format is
supported.

<iPixelFormat> is an index that specifies the pixel format.The
pixel formats that a device context supports are identified by
positive one - based integer indexes.

<iLayerPlane> specifies which plane is being queried.Positive
values of <iLayerPlane> identify overlay planes, where 1 is the
first overlay plane over the main plane, 2 is the second overlay
plane over the first overlay plane, and so on.Negative values
identify underlay planes, where - 1 is the first underlay plane under
the main plane, -2 is the second underlay plane under the first
underlay plane and so on.Use zero for the main plane.

<nAttributes> number of attributes being queried.

<piAttributes> list containing an array of pixel format attribute
identifiers which specify the attributes to be queried.The values
accepted are the same as for wglGetPixelFormatAttribivARB.

<pfValues> is a pointer to a buffer into which the results of the
query will be placed.Integer attribute values are converted
floating point The caller must allocate this array and it must have
at least at least <nAttributes> entries.

If the function succeeds, the return value is TRUE.If the function
fails, the return value is FALSE.To get extended error information,
call GetLastError.

An error is generated if <piAttributes> contains an invalid
attribute, if <iPixelFormat> is not a positive integer or is larger
than the number of pixel formats, if <iLayerPlane> doesn't refer to
an existing layer plane, or if <hdc> is invalid.

If FALSE is returned, the contents of <pfValues> are undefined.

Supported Pixel Formats

The maximum index of the pixel formats which can be referenced by
the standard pixel format functions is returned by a successful call
to DescribePixelFormat.This may be less than the maximum index of
the pixel formats which can be referenced by
wglGetPixelFormatAttribivARB and wglGetPixelFormatAttribfvARB.
(determined by querying WGL_NUMBER_PIXEL_FORMATS_ARB).

The pixel format of a "displayable" object(e.g.window, bitmap) is
specified by passing its index to SetPixelFormat.Therefore, pixel
formats which cannot be referenced by the standard pixel format
functions are "non displayable".

Indices are assigned to pixel formats in the following order :

1. Accelerated pixel formats that are displayable

2. Accelerated pixel formats that are displayable and which have
extended attributes

3. Generic pixel formats

4. Accelerated pixel formats that are non displayable

ChoosePixelFormat will never select pixel formats from either group
2 or group 4. Each pixel format in group 2 is required to appear
identical to some pixel format in group 1 when queried by
DescribePixelFormat.Consequently, ChoosePixelFormat will always
select a format from group 1 when it might otherwise have selected a
format from group 2. Pixel formats in group 4 cannot be accessed by
ChoosePixelFormat at all.

SetPixelFormat and DescribePixelFormat will only accept pixel
formats from groups 1 - 3. If a non - displayable pixel format is
specified to SetPixelFormat or DescribePixelFormat an error will
result.These pixel formats are only for use with WGL extensions,
such as WGLCreatePbufferARB.

The following function may be used to select from among all of the
available pixel formats(including both accelerated and generic
						formats and non - displayable formats).This function accepts
	attributes for the main planes.A list of pixel formats that match
	the specified attributes is returned with the "best" pixel formats
	at the start of the list(order is device dependent).

	BOOL wglChoosePixelFormatARB(HDC hdc,
								 const int *piAttribIList,
								 const FLOAT *pfAttribFList,
								 UINT nMaxFormats,
								 int *piFormats,
								 UINT *nNumFormats);

<hdc> specifies the device context.

<piAttribIList> specifies a list of attribute{ type, value } pairs
containing integer attribute values.All the attributes in
<piAttribIList> are followed by the corresponding desired value.The
list is terminated with 0. If <piAttribList> is NULL then the result
is the same as if <piAttribList> was empty.

<pfAttribFList> specifies a list of attribute{ type, value } pairs
containing floating point attribute values.All the attributes in
<pfAttribFList> are followed by the corresponding desired value.The
list is terminated with 0. If <pfAttribList> is NULL then the result
is the same as if <pfAttribList> was empty.

<nMaxFormats> specifies the maximum number of pixel formats to be
returned.

<piFormats> points to an array of returned indices of the matching
pixel formats.The best pixel formats(i.e., closest match and best
									 format for the hardware) are at the head of the list.The caller
	must allocate this array and it must have at least <nMaxFormats>
	entries.

	<nNumFormats> returns the number of matching formats.The returned
	value is guaranteed to be no larger than <nMaxFormats>.

	If the function succeeds, the return value is TRUE.If the function
	fails the return value is FALSE.To get extended error information,
	call GetLastError.If no matching formats are found then nNumFormats
	is set to zero and the function returns TRUE.

	If FALSE is returned, the contents of <piFormats> are undefined.

	wglChoosePixelFormatARB selects pixel formats to return based on the
	attribute values specified in <piAttribIList> and <pfAttribFList>.
	Some attribute values must match the pixel format value exactly when
	the attribute is specified while others specify a minimum criteria,
	meaning that the pixel format value must meet or exceed the
	specified value.See the table below for details.


	Attribute                      Type        Match Criteria

	WGL_DRAW_TO_WINDOW_ARB         boolean     exact
	WGL_DRAW_TO_BITMAP_ARB         boolean     exact
	WGL_ACCELERATION_ARB           enum        exact
	WGL_NEED_PALETTE_ARB           boolean     exact
	WGL_NEED_SYSTEM_PALETTE_ARB    boolean     exact
	WGL_SWAP_LAYER_BUFFERS_ARB     boolean     exact
	WGL_SWAP_METHOD_ARB            enum        exact
	WGL_NUMBER_OVERLAYS_ARB        integer     minimum
	WGL_NUMBER_UNDERLAYS_ARB       integer     minimum
	WGL_SHARE_DEPTH_ARB            boolean     exact
	WGL_SHARE_STENCIL_ARB          boolean     exact
	WGL_SHARE_ACCUM_ARB            boolean     exact
	WGL_SUPPORT_GDI_ARB            boolean     exact
	WGL_SUPPORT_OPENGL_ARB         boolean     exact
	WGL_DOUBLE_BUFFER_ARB          boolean     exact
	WGL_STEREO_ARB                 boolean     exact
	WGL_PIXEL_TYPE_ARB             enum        exact
	WGL_COLOR_BITS_ARB             integer     minimum
	WGL_RED_BITS_ARB               integer     minimum
	WGL_GREEN_BITS_ARB             integer     minimum
	WGL_BLUE_BITS_ARB              integer     minimum
	WGL_ALPHA_BITS_ARB             integer     minimum
	WGL_ACCUM_BITS_ARB             integer     minimum
	WGL_ACCUM_RED_BITS_ARB         integer     minimum
	WGL_ACCUM_GREEN_BITS_ARB       integer     minimum
	WGL_ACCUM_BLUE_BITS_ARB        integer     minimum
	WGL_ACCUM_ALPHA_BITS_ARB       integer     minimum
	WGL_DEPTH_BITS_ARB             integer     minimum
	WGL_STENCIL_BITS_ARB           integer     minimum
	WGL_AUX_BUFFERS_ARB            integer     minimum

	All attributes except WGL_NUMBER_OVERLAYS_ARB, WGL_NUMBER_UNDERLAYS_ARB,
	WGL_SHARE_DEPTH_ARB, WGL_SHARE_STENCIL_ARB, and WGL_SHARE_ACCUM_ARB
	apply to the main planes and not to any layer planes.If
	WGL_SHARE_DEPTH_ARB, WGL_SHARE_STENCIL_ARB, and WGL_SHARE_ACCUM_ARB are
	specified in either <piAttribList> or <pfAttribList>, then a pixel
	format will only be selected if it has no overlays or underlays or if
	all of its overlays and underlays match the specified value.
	Applications that need to find a pixel format that supports a layer
	plane with other buffer attributes(such as WGL_SUPPORT_OPENGL_ARB set
									   to TRUE), must go through the list that is returned and call
	wglGetPixelFormatAttrib*vARB to find one with the appropriate
	attributes.

	Attributes that are specified in neither <piAttribIList> nor
	<pfAttribFList> are ignored(i.e., they are not looked at during the
								selection process).In addition the following attributes are always
	ignored, even if specified: WGL_NUMBER_PIXEL_FORMATS_ARB,
	WGL_RED_SHIFT_ARB, WGL_GREEN_SHIFT_ARB, WGL_BLUE_SHIFT_ARB,
	WGL_ALPHA_SHIFT_ARB, WGL_TRANSPARENT_ARB,
	WGL_TRANSPARENT_RED_VALUE_ARB, WGL_TRANSPARENT_GREEN_VALUE_ARB,
	WGL_TRANSPARENT_BLUE_VALUE_ARB, WGL_TRANSPARENT_ALPHA_VALUE_ARB, and
	WGL_TRANSPARENT_INDEX_ARB.

	If both <piAttribIList> and <pfAttribFList> are NULL or empty then all
	pixel formats for this device are returned.

	An error is generated if <piAttribIList> or <pfAttribFList> contain an
	invalid attribute or if <hdc> is invalid.

	Although it is not an error, wglChoosePixelFormat and
	wglChoosePixelFormatARB should not be used together.It is not necessary
	to change existing OpenGL programs but application writers should use
	wglChoosePixelFormatARB whenever possible.New pixel format attributes
	introduced by extensions(such as the number of multisample buffers)
	will only be known to the new calls, wglChoosePixelFormatARB and
	wglGetPixelFormatAttrib*vARB..

	New State

	None

	New Implementation Dependent State

	None

	Dependencies on WGL_ARB_extensions_string

	Because there is no way to extend WGL, these calls are defined in the
	ICD and can be called by obtaining the address with wglGetProcAddress.
	Because this extension is a WGL extension, it is not included in the
	extension string returned by glGetString.Its existence can be
	determined with the WGL_ARB_extensions_string extension.

	Revision History

	Changes from EXT_pixel_format :

*Added WGL prefix to name to avoid possible name collisions
* EXT suffix changed to ARB
* Updated to new template, adding contact, status and revision sections
* Version is no longer an RCS version
* Attribute list passed to wglGetPixelFormatAttrib*v is type const
* Separate red, green, blue, alpha and index transparent values
* WGL_SWAP_LAYER_BUFFERS and WGL_SWAP_METHOD values defined for single
buffered pixel formats
* Array of return values for wglGetPixelFormatAttrib*v and
wglChoosePixelFormatARB is undefined if function fails
* Error returned if iPixelFormat is zero or negative in
wglGetPixelFormat*v
* Under "Supported Pixel Formats", indicate that SetPixelFormat and
DescribePixelFormat do not accept non displayable pixel formats.
Passing one in results in an error
* If either piAttribIList of pfAttribFList are NULL when
wglChoosePixelFormatARB is called then it is as if they were empty
* Clarify that wglChoosePixelFormatARB returns TRUE even if no matching
formats found
* wglChoosePixelFormatARB will only match an overlay attribute(eg,
															   WGL_SHARE_DEPTH_ARB) if there are no overlay planes or if all
	overlay / underlay plane attributes match the specified criteria
	* Be careful about using term hardware(change to pixel format where
										   appropriate)
	* wglChoosePixelFormatARB now ignores the following attributes(in
																   addition to WGL_NUMBER_PIXEL_FORMATS_ARB) : WGL_ * _SHIFT_ARB,
	WGL_TRANSPARENT_ARB, WGL_TRANSPARENT_*_VALUE_ARB.
	* Clarify that new pixel format attributes(eg, attributes introduced by
											   extensions such as multisampling) are only known to the new pixel
	format calls, wglChoosePixelFormatARB and wglGetPixelFormat*vARB.
	* Add dependency on WGL_ARB_extensions_string

	03 / 12 / 2002  1.1
	- Updated contact information.

	07 / 29 / 2008  1.2
	- Updated contact information.

	03 / 25 / 2009  1.3
	- mjk: Fixed <nNumFormats> description
*/

// SOURCE: https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
// COPIED DATE: 12/27/2018

/*
Name

WGL_ARB_create_context
WGL_ARB_create_context_profile

Name Strings

WGL_ARB_create_context
WGL_ARB_create_context_profile

Contact

Jon Leech(jon 'at' alumni.caltech.edu)
Michael Gold, NVIDIA(gold 'at' nvidia.com)

Notice

Copyright(c) 2009 - 2013 The Khronos Group Inc.Copyright terms at
http ://www.khronos.org/registry/speccopyright.html

IP Status

No known IP claims.

Status

Complete.Approved by the ARB on July 28, 2009.
Approved by the Khronos Board of Promoters on August 28, 2009.

Version

July 28, 2009 (version 20)

Number

ARB Extension #55 (WGL_ARB_create_context)
ARB Extension #74 (WGL_ARB_create_context_profile)

Dependencies

WGL_ARB_extensions_string is required.

The presence of an OpenGL 3.2 or later implementation determines
whether or not WGL_ARB_create_context_profile is required.

Overview

With the advent of new versions of OpenGL which deprecate features
and/or break backward compatibility with older versions, there is a
need and desire to indicate at context creation which interface will
be used.These extensions add a new context creation routine with
attributes specifying the GL version and context properties
requested for the context, and additionally add an attribute
specifying the GL profile requested for a context of OpenGL 3.2 or
later.

New Procedures and Functions

HGLRC wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext,
								 const int *attribList)

	New Tokens

	Accepted as an attribute name in <*attribList> :
*/

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

/*
Accepted as bits in the attribute value for WGL_CONTEXT_FLAGS in
<*attribList> :
*/
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

// Accepted as bits in the attribute value for
// WGL_CONTEXT_PROFILE_MASK_ARB in <*attribList> :

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

// New errors returned by GetLastError :

#define ERROR_INVALID_VERSION_ARB               0x2095
#define ERROR_INVALID_PROFILE_ARB               0x2096

/*
Additions to the OpenGL / GLX / GLX Protocol Specificaitons

None.This specification is written for WGL.

Additions to the WGL specification

Context Creation
----------------

To create an OpenGL rendering context, call

HGLRC wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext,
								 const int *attribList);

If wglCreateContextAttribsARB succeeds, it initializes the context
to the initial state defined by the OpenGL specification, and
returns a handle to it.The handle can be used(via wglMakeCurrent
											  or wglMakeContextCurrentARB) with any HDC sharing the same pixel
	format as <hDC>, and created on the same device, subject to
	constraints imposed by the API version.

	If <hshareContext> is not 0, then all shareable data(excluding
														 OpenGL texture objects named 0) will be shared by <hshareContext>,
	all other contexts <hshareContext> already shares with, and the
	newly created context.An arbitrary number of contexts can share
	data in this fashion.

	<attribList> specifies a list of attributes for the context.The
	list consists of a sequence of <name, value> pairs terminated by the
	value 0. If an attribute is not specified in <attribList>, then the
	default value specified below is used instead.If an attribute is
	specified more than once, then the last value specified is used.

	<attribList> may be NULL or empty(first attribute is 0), in which
	case all attributes assume their default values as described below.

	The attribute names WGL_CONTEXT_MAJOR_VERSION_ARB and
	WGL_CONTEXT_MINOR_VERSION_ARB request an OpenGL context supporting
	the specified version of the API.If successful, the context
	returned must be backwards compatible with the context requested.
	Backwards compatibility is determined as follows :

If a version less than or equal to 3.0 is requested, the context
returned may implement any of the following versions :

*Any version no less than that requested and no greater than 3.0.
* Version 3.1, if the GL_ARB_compatibility extension is also
implemented.
* The compatibility profile of version 3.2 or greater.

If version 3.1 is requested, the context returned may implement
any of the following versions :

*Version 3.1.The GL_ARB_compatibility extension may or may not
be implemented, as determined by the implementation.
* The core profile of version 3.2 or greater.

If version 3.2 or greater is requested, the context returned may
implement any of the following versions :

*The requested profile of the requested version.
* The requested profile of any later version, so long as no
features have been removed from that later version and profile.

Querying the GL_VERSION string with glGetString(or the
												GL_MAJOR_VERSION and GL_MINOR_VERSION values with glGetIntegerv, in
												a 3.0 or later context) will return the actual version supported by
	a context.

	The default values for WGL_CONTEXT_MAJOR_VERSION_ARB and
	WGL_CONTEXT_MINOR_VERSION_ARB are 1 and 0 respectively.In this
	case, implementations will typically return the most recent version
	of OpenGL they support which is backwards compatible with OpenGL 1.0
	(e.g. 3.0, 3.1 + GL_ARB_compatibility, or 3.2 compatibility profile)

	The attribute name WGL_CONTEXT_PROFILE_MASK_ARB requests an OpenGL
	context supporting a specific <profile> of the API.If the
	WGL_CONTEXT_CORE_PROFILE_BIT_ARB bit is set in the attribute value,
	then a context implementing the <core> profile of OpenGL is
	returned.If the WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB bit is
	set, then a context implementing the <compatibility> profile is
	returned.If the requested OpenGL version is less than 3.2,
	WGL_CONTEXT_PROFILE_MASK_ARB is ignored and the functionality of the
	context is determined solely by the requested version.

	Querying the value of GL_CONTEXT_PROFILE_MASK with glGetIntegerv
	will return the profile mask used to create the context.This query
	is only supported in an OpenGL 3.2 or later context.

	The default value for WGL_CONTEXT_PROFILE_MASK_ARB is
	WGL_CONTEXT_CORE_PROFILE_BIT_ARB.All OpenGL 3.2 implementations are
	required to implement the core profile, but implementation of the
	compatibility profile is optional.

	If the core profile is requested, then the context returned cannot
	implement functionality defined only by the compatibility profile.

	The attribute name WGL_CONTEXT_LAYER_PLANE specifies the layer plane
	that the rendering context is bound to.The attribute value has the
	same meaning and allowed values as the <iLayerPlane> parameter of
	wglCreateLayerContext.The default value for WGL_CONTEXT_LAYER_PLANE
	is 0 (the main plane).

	The attribute name WGL_CONTEXT_FLAGS_ARB specifies a set of flag
	bits affecting the rendering context.

	If the WGL_CONTEXT_DEBUG_BIT_ARB flag bit is set in
	WGL_CONTEXT_FLAGS_ARB, then a <debug context> will be created.Debug
	contexts are intended for use during application development, and
	provide additional runtime checking, validation, and logging
	functionality while possibly incurring performance penalties.The
	additional functionality provided by debug contexts may vary
	according to the implementation(fn).In some cases a debug context
	may be identical to a non - debug context.
	[fn:The ARB Ecosystem TSG is still defining the expected and
	required features of debug contexts.]

If the WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB is set in
WGL_CONTEXT_FLAGS_ARB, then a <forward - compatible> context will be
created.Forward - compatible contexts are defined only for OpenGL
versions 3.0 and later.They must not support functionality marked
as <deprecated> by that version of the API, while a
non - forward - compatible context must support all functionality in
that version, deprecated or not.

The default value of WGL_CONTEXT_FLAGS_ARB is 0.

On failure wglCreateContextAttribsARB returns NULL.Extended error
information can be obtained with GetLastError.Conditions that cause
failure include :

*If attributes WGL_CONTEXT_MAJOR_VERSION_ARB and
WGL_CONTEXT_MINOR_VERSION_ARB, when considered together with
WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, specify an OpenGL version
and feature set that are not defined, then ERROR_INVALID_VERSION_ARB
is generated.

The defined versions of OpenGL at the time of writing are OpenGL
1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 2.0, 2.1, 3.0, 3.1, and 3.2.Feature
deprecation was introduced with OpenGL 3.0, so forward - compatible
contexts may only be requested for OpenGL 3.0 and above.Thus,
examples of invalid combinations of attributes include :

-Major version < 1 or > 3
- Major version == 1 and minor version < 0 or > 5
- Major version == 2 and minor version < 0 or > 1
- Major version == 3 and minor version > 2
- Forward - compatible flag set and major version < 3

	Because the purpose of forward - compatible contexts is to allow
	application development on a specific OpenGL version with the
	knowledge that the app will run on a future version, context
	creation will fail if WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB is set
	and the context version returned cannot implement exactly the
	requested version.

	* If attribute WGL_CONTEXT_PROFILE_MASK_ARB has no bits set; has any
	bits set other than WGL_CONTEXT_CORE_PROFILE_BIT_ARB and
	WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB; has more than one of
	these bits set; or if the implementation does not supported the
	requested profile, then ERROR_INVALID_PROFILE_ARB is generated.

	* If <hShareContext> is neither zero nor a valid context handle, then
	ERROR_INVALID_OPERATION is generated.

	* If <hDC> is not a valid device context handle, then
	ERROR_DC_NOT_FOUND is generated.

	* If the pixel format associated with <hDC> does not support OpenGL
	contexts providing the requested API major and minor version,
	forward - compatible flag, and/or debug context flag, then
	ERROR_INVALID_PIXEL_FORMAT is generated.

	* If the OpenGL server context state for <hShareContext> exists in an
	address space that cannot be shared with the newly created context,
	if <shareContext> was created on a different device context than the
		one referenced by <hDC>, or if the contexts are otherwise
		incompatible(for example, one context being associated with a
					 hardware device driver and the other with a software renderer), then
		ERROR_INVALID_OPERATION is generated.

		* If the server does not have enough resources to allocate the new
		context, then ERROR_NO_SYSTEM_RESOURCES is generated.

		* If an unrecognized attribute is present in <attribList>,
		then ERROR_INVALID_PARAMETER is generated.

		Legacy Context Creation
		---------------------- -

		When this extension is supported, calling

		wglCreateContext(hdc)

		is equivalent to calling

		wglCreateContextAttribs(hdc, 0, NULL).

		Calling

		wglCreateLayerContext(hdc, layer)

		is equivalent to calling

		wglCreateContextAttribs(hdc, 0,
								{ WGL_CONTEXT_LAYER_PLANE, layer, 0 }) .

		The legacy context creation routines can only return OpenGL 3.1
		contexts if the GL_ARB_compatibility extension is supported, and can
		only return OpenGL 3.2 or greater contexts implementing the
		compatibility profile.This ensures compatibility for existing
		applications.However, 3.0 - aware applications are encouraged to use
		wglCreateContextAttribsARB instead of the legacy routines.

		Binding a Context to a Surface
		------------------------------

		(add to the description of wglMakeCurrent and
		 wglMakeContextCurrentARB)

		"If the OpenGL context version of <hglrc> is 3.0 or above, and if
		either the <hdc> parameter of wglMakeCurrent is NULL, or both of the
		<hDrawDC> and <hReadDC> parameters of wglMakeContextCurrentARB are
		NULL, then the context is made current, but with no default
		framebuffer defined.The effects of having no default framebuffer on
		the GL are defined in Chapter 4 of the OpenGL 3.0 Specification.

		The first time <ctx> is made current, the viewport and scissor
		dimensions are set to the size of the draw drawable(as though
															glViewport(0, 0, w, h) and glScissor(0, 0, w, h) were called, where <w>
															and <h> are the width and height of the drawable, respectively).If
		the first time <ctx> is made current, it is without a default
		framebuffer, then the viewport and scissor regions are set as though
		glViewport(0, 0, 0, 0) and glScissor(0, 0, 0, 0) were called."
		However, the viewport and scissor dimensions are not modified when
		<ctx> is subsequently made current; it is the client's
		responsibility to reset the viewport and scissor in this case."

		Sharing Objects Between Contexts
		--------------------------------

		(add to the description of wglShareLists)

		Future versions of OpenGL may only support being added to a share
		group at context creation time.Specifying such a version of a
		context as either the <hglrc1> or <hglrc2> arguments to
		wglShareLists will fail.wglShareLists will return FALSE, and
		GetLastError will return ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB.

		Dependencies on WGL_ARB_extensions_string

		Because there is no way to extend wgl, these calls are defined in
		the ICD and can be called by obtaining the address with
		wglGetProcAddress.Because this extension is a WGL extension, it is
		not included in the GL_EXTENSIONS string.Its existence can be
		determined with the WGL_ARB_extensions_string extension.

		Dependencies on WGL_ARB_create_context

		If WGL_ARB_create_context_profile is supported, then
		WGL_ARB_create_context must also be supported.

		Dependencies on OpenGL 3.2 and later OpenGL versions

		If WGL_ARB_create_context is supported, and if the OpenGL
		implementation supports OpenGL 3.2 or later, then
		WGL_ARB_create_context_profile must also be supported.If the OpenGL
		implementation does not support OpenGL 3.2 or later, then
		WGL_ARB_create_context_profile may or may not be supported.

		If WGL_ARB_create_context_profile is not supported, then the
		WGL_CONTEXT_PROFILE_MASK_ARB attribute, the
		WGL_CONTEXT_CORE_PROFILE_BIT_ARB and
		WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB bits in that attribute,
		and the ERROR_INVALID_PROFILE_ARB error are not defined, and
		specifying the attribute in <attribList> attribute will generate an
		error.

		New State

		None

		New Implementation Dependent State

		None

		Conformance Tests

		TBD

		Sample Code

		TBD

		Issues

		1) Can different GL context versions share data ?

		PROPOSED : Yes, with restrictions as defined by the supported feature
		sets.For example, program and shader objects cannot be shared with
		OpenGL 1.x contexts, which do not support them.

		NOTE : When the new object model is introduced, sharing must be
		established at creation time, since the object handle namespace is
		also shared.wglShareLists would therefore fail if either context
		parameter to it were to be a context supporting the new object
		model.

		2) Should we introduce a token WGL_NONE to terminate attribute lists,
		instead of using the value 0, by analogy to GLX and EGL ?

		RESOLVED : No.

		3) Should all GL context versions be able to bind to all the same pixel
		formats ?

		RESOLVED : unlike EGL, we don't expose the version restriction in the
		pixel format.Instead, wglMakeCurrent may fail when binding contexts
		to surfaces if the surface type is unrecognized by that context
		version.No specific examples exist at present, but in a
		hypothetical future version of the API supporting only the new
		object model, such an API probably could not bind to pbuffer
		surfaces.

		4) Should there be a way to make a context current without binding
		it to a window system drawable at the same time ?

		RESOLVED : Yes, but only in OpenGL 3.0 and later.This results in a
		context with an invalid default framebuffer, the meaning of which is
		defined in the OpenGL 3.0 specification.

		NOTE : Apparently on Windows, opengl32.dll makes use of the drawable
		argument to identify the namespace of the driver, so we may not be
		able to work around it.

		5) Should we add a token to the attrib list for pixel format selection
		to indicate the GL context version, instead of introducing a new
		entry point for context creation ?

		RESOLVED : No.We want pixel formats to be portable between
		feature - incompatible context versions, so that drawables and GL
		objects that make sense to share, may be shared between such
		contexts.This is important for code migration and backward
		compatibility.

		6) Do we need a "can be shared" parameter at context creation time ?

		RESOLVED : No.All contexts have potential to share objects(whether
																   they actually can share depends on context version and feature sets,
																   and the address space they're created in). Savings from saying a
																   context cannot share objects are unclear but probably minimal.Such
																   a parameter would default to TRUE if it existed.

																   7) How are "debug contexts" created ?

		RESOLVED : by specifying the WGL_CONTEXT_DEBUG_BIT_ARB flag in the
		WGL_CONTEXT_FLAGS_ARB attribute value.

		DISCUSSION : For the most part, the behavior of debug contexts is
		being determined by the ARB Ecosystem TSG.However, whatever their
		behavior, it's clear that "debug contexts" may differ at a basic
		level from regular contexts.Debug contexts might even go down a
		completely separate driver path.So it's essential to know at
		context creation time if a debug context is required.

		8) Is wglCreateContextVersion a good name ?

		Ian Romanick suggests wglCreateContextAttribs instead.Changed in
		version 12.

		9) Should the context version be named with separate major and minor
		number attributes, or as a single(major * 100 + minor * 10) number as
		in GLSL(e.g.version == 220 < ->major = 2, minor = 2) ?

		John Kessenich argues for consistency with the existing GLSL naming
		scheme.

		10) How are profiles specified ? Why is the profile attribute a bitmask
		rather than an enum ?

		RESOLVED : Profiles are specified as individual bits passed in a
		bitmask attribute.This attribute is ignored unless the OpenGL
		version requested defines profiles(OpenGL 3.2 and later).While
		only a single profile can be specified today, it is not unlikely
		that eventually there will be profiles defined than can be
		implemented in the same context.In that case we will need to be
		able to specify multiple profiles at context creation and in the GL
		context profile query, for which a bitmask is appropriate.

		11) Why is the value of WGL_CONTEXT_PROFILE_MASK_ARB in the GL
		core enumerant range ?

		RESOLVED : The token value for the context profile mask and for the
		context profile bits should be shared by GL, GLX, and WGL to ease
		implementation and reduce programming errors.These values are drawn
		from the GL token ranges.

		12) What errors can result from requesting a profile ?

		RESOLVED : Only a single profile can currently be requested.
		Requesting no profile, more than one profile, or an unrecognized
		profile are all errors.

		13) How does the profile request interact with older GL versions ?

		RESOLVED : It does not.The profile mask is ignored unless the
		requested GL version is >= 3.2.

		DISCUSSION : Bruce had proposed using the profile mask to control
		whether or not GL_ARB_compatibility was provided in earlier GL
		versions, but general feeling was that apps had already had to sort
		this out, and it's difficult to see how to define a default value
		that can always be implemented(core profile) without changing the
		existing behavior of wglCreateContextAttribsARB on implementations
		which implement GL_ARB_compatibility by default when an OpenGL 3.1
		context is requested.

		14) How does the profile request interact with the forward compatibility
		bit ?

		RESOLVED : Nothing special need be done here.If the forward
		compatibility bit is set, the returned context must not implement
		deprecated functionality in that profile(nothing is deprecated
												 from the OpenGL 3.2 compatibility profile, so a forward - compatible
												 compatibility profile has exactly the same functionality as
												 a non - forward - compatible compatibility profile, while a
												 forward - compatible core profile would leave out only the handful
												 of features - wide lines and two queries - deprecated from core
												 OpenGL 3.2).

		15) How is context sharing between contexts of different versions
		handled ?

		RESOLVED : It's up to the implementation whether or not to allow
		this, and to define behavior when shared objects include state or
		behaviors not described by one of the contexts sharing them(for
																	example, textures with nonzero width borders shared by 3.2 core and
																	compatibility profile contexts).

		16) Is the GL_ARB_compatibility extension equivalent to the
		compatibility profile ?

		RESOLVED : No.The extension refers specifically to functionality
		removed from OpenGL 3.1 and restored by this extension.The
		compatibility profile also restores this functionality, but may
		define additional interactions between new features in version 3.N
		and the functionality removed in OpenGL 3.1.

		In other words, the extension is a subset of the compatibility
		profile.A context implementing the compatibility profile for OpenGL
		3.2 or greater may(and probably will) choose to export
		GL_ARB_compatibility, but this is not required.

		17) If the core profile is requested, what additional functionality
		can be provided ?

		RESOLVED : Functionality defined only by the compatibility profile
		cannot be implemented by a context supporting the core profile.For
		example, if a 3.2 core profile is requested, that context cannot
		implement the GL_ARB_compatibility extension.However, any desired
		new functionality may be exposed via vendor or ARB extensions in the
		normal fashion.


		Revision History

		Version 20, 2009 / 07 / 28 - Correct issue 14 to note that the 3.2
		compatibility profile does not deprecated anything.

		Version 19, 2009 / 07 / 27 - Add profiled context creation functionality
		(WGL_CONTEXT_PROFILE_MASK_ARB and the associated bit tokens, error
		 code, and issues 10 - 16).Clarify that if the core profile is
		requested, the returned context cannot restore functionality from
		the compatibility profile(issue 17 and corresponding spec
								  language).Add ERROR_INVALID_PARAMETER error if an unrecognized
		attribute is specified.Specify the interdependencies between the
		two WGL extensions defined here and OpenGL 3.2, including the
		requirement that WGL_ARB_create_context_profile be supported if an
		OpenGL 3.2 or later implementation is supported.

		Version 18, 2009 / 07 / 24 - Specify that repeated attribute values
		always result in the last specified value being used.

		Version 17, 2009 / 07 / 23 - Update the extension to clarify
		interactions with OpenGL 3.1 and greater, allow returning 3.0 and
		greater contexts from the legacy context creation routines so long
		as they're backwards compatible, and mandate that requesting a
		forward - compatible context always return exactly the requested
		version(bugs 5112, 5349).

		Version 16, 2008 / 08 / 21 - Add initial viewport / scissor setting
		behavior(cloned from GLX, including the case when no default
				 framebuffer is made current.

				 Version 15, 2008 / 08 / 10 - Assign enumerant values and extension
				 number.

				 Version 14, 2008 / 07 / 10 - Add trailing zero to array attributes in
				 wglCreateLayerContext example(Pat Brown).

				 Version 13, 2008 / 07 / 03 - Update versioning behavior so asking for <=
				 2.1 will return <= 2.1, asking for 3.0 will return 3.0, and noting
				 that we must update interactions with GL once 3.1 is defined.
				 Mandated behavior of legacy context creation APIs when this
				 extension is supported(bug 3595).

				 Version 12, 2008 / 07 / 02 - Rename WGL_PREVIEW_BIT to
				 WGL_CONTEXT_FORWARD_COMPATIBLE_BIT and other replace "preview
				 context" with "forward - compatible context". Rename
				 wglCreateContextVersion to wglCreateContextAttribs.

				 Version 11, 2008 / 06 / 12 - Use ARB suffix.Raise ERROR_INVALID_VERSION
				 instead of ERROR_INVALID_DATA on a bad version / preview combination.

				 Version 10, 2008 / 04 / 08 - Changed "lite" to "preview" (still open for
																		   discussion however), and changed version "2.2" to "3.0" following
				 the marketing TSG recommendation.Changed definition of preview
				 contexts so that deprecated functionality *must not* be supported.
				 Added issue 10 about how to handle profiles.

				 Version 9, 2008 / 02 / 07 - Added version naming issue 9.

				 Version 8, 2008 / 02 / 07 - Added naming issue 8. Removed reference to
				 GL version 3 in "Sharing Objects Between Contexts".

				 Version 7, 2008 / 02 / 01 - Rewrote issues list in light of new API
				 direction, and reopened some issues.Added separate major and minor
				 version attributes and "lite" API flag.

				 Version 6, 2007 / 05 / 22 - Resolve issue 4, although the exact
				 mechanism is still unknown.

				 Version 5, 2007 / 04 / 09 - Added WGL_CONTEXT_FLAGS attribute and
				 WGL_CONTEXT_DEBUG_BIT flag, and a high - level description of the
				 concept of "debug contexts".

				 Version 4, 2007 / 02 / 06 - Removed CreateContextVersion error when GL2
				 and Longs Peak contexts are placed in the same share group, and
				 referred to the GL_EXT_bind_image extension in issue 1. Added issues
				 6 ("can be shared" attribute) and 7 (creation of debug contexts) per
				 discussion at the Atlanta F2F ARB meeting.Added error condition for
				 wglShareLists if passed a Longs Peak context handle.

				 Version 3, 2007 / 01 / 22 - added issue 5.

				 Version 2, 2007 / 01 / 17 - change to the same style as
				 eglCreateContext, using an attribute list.Add wglMakeCurrent
				 language disallowing current 3.x contexts bound to pbuffers, and
				 allowing current 3.x context without a window - system provided
				 framebuffer.

				 Version 1, 2007 / 1 / 17 - First draft
*/