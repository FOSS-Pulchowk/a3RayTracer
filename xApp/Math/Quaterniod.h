#include "Common/Core.h"
#include "Matrix4.h"

struct quat
{
    union {
        struct
        {
            f32 r, i, j, k;
        };
        struct
        {
            f32 w, x, y, z;
        };
        struct
        {
            f32 d, a, b, c;
        };
    };

    // Constructs Unit Quaternion
    quat();
    quat(const quat &q);
    quat(f32 r, f32 i, f32 j, f32 k);

    quat &operator=(const quat &q);

    /*
			- Using these operators may break the rotation Quaternion
			- It's a good idea to Normalize after performing these operations
			- Normalization is not done inside of these methods
		*/
    quat &operator+=(const quat &q);
    quat &operator-=(const quat &q);
    quat &operator*=(const quat &q);

    // Get length of the quaternion
    static f32 Length(const quat &q);

    // Get conjugate of th quaternion
    static quat Conjugate(const quat &q);

    // Normalizes the quaternions, use this when you not sure if the Quaternion in unit Quaternion
    static quat Normalize(const quat &q);

    // Dot product of the two given quaternions
    static f32 Dot(const quat &q1, const quat &q2);

    // Quaternion-Quaternion multiplication, can be used to stack Rotation
    friend quat operator*(const quat &lhs, const quat &rhs);
};
