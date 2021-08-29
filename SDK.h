#pragma once
#define DECL_ALIGN(x) __declspec(align(x))
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN16_POST DECL_ALIGN(16)
#include <ostream>
class Vector3
{
public:
    Vector3(void)
    {
        Invalidate();
    }
    Vector3(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }
    Vector3(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }
    bool IsValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    void Invalidate()
    {
        x = y = z = std::numeric_limits<float>::infinity();
    }

    float& operator[](int i)
    {
        return ((float*)this)[i];
    }
    float operator[](int i) const
    {
        return ((float*)this)[i];
    }

    void Zero()
    {
        x = y = z = 0.0f;
    }

    bool operator==(const Vector3& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
    bool operator!=(const Vector3& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    Vector3& operator+=(const Vector3& v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vector3& operator-=(const Vector3& v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    Vector3& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
    Vector3& operator*=(const Vector3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    Vector3& operator/=(const Vector3& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    Vector3& operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
    Vector3& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    Vector3& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    std::ostream& operator<< (std::ostream& output) {
        output << "X: " << x << " Y: " << y << " Z: " << z;
        return output;
    }
    // @param Vector to calculate dot product with
    float DotProduct(Vector3 vec) {
        return x * vec.x + y * vec.y + z * vec.z;
    }

    float Magnitude() {
        return (float)sqrtf(x * x + y * y + z * z);
    }
    //@param Vector to calculate distance to
    float Distance(Vector3 vec) {
        return (*this - vec).Magnitude();
    }

    Vector3 Normalize() {
        return *this / Magnitude();
    }
    float LengthSqr(void) const
    {
        return (x * x + y * y + z * z);
    }
    float Length2D() const
    {
        return sqrt(x * x + y * y);
    }
    /*
 Moves a certain percent between to points
 @param destination to move to
 @param percent to move towards the destination
 @return location of partial movement towards destination
*/
    Vector3 Lerp(Vector3 vec, float percent) {
        Vector3 Dist = (*this - vec);
        if (Dist.y < -180) {
            Dist.y += 360;
        }
        else if (Dist.y > 180) {
            Dist.y -= 360;
        }
        Dist = Dist * percent;
        return Dist;
    }

    Vector3& operator=(const Vector3& vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    Vector3 operator-(void) const
    {
        return Vector3(-x, -y, -z);
    }
    Vector3 operator+(const Vector3& v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    Vector3 operator-(const Vector3& v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    Vector3 operator*(float fl) const
    {
        return Vector3(x * fl, y * fl, z * fl);
    }
    Vector3 operator*(const Vector3& v) const
    {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }
    Vector3 operator/(float fl) const
    {
        return Vector3(x / fl, y / fl, z / fl);
    }
    Vector3 operator/(const Vector3& v) const
    {
        return Vector3(x / v.x, y / v.y, z / v.z);
    }

    float x, y, z;
};

inline Vector3 operator*(float lhs, const Vector3& rhs)
{
    return rhs * lhs;
}
inline Vector3 operator/(float lhs, const Vector3& rhs)
{
    return rhs / lhs;
}

class ALIGN16 VectorAligned : public Vector3
{
public:
    inline VectorAligned(void) {};

    inline VectorAligned(Vector3 X, Vector3 Y, Vector3 Z)
    {
        //Init(X, Y, Z);
    }

    explicit VectorAligned(const Vector3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
    }

    VectorAligned& operator=(const Vector3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    float w;	// this space is used anyway
} ALIGN16_POST;

struct Bone
{
    char __paddingA[12];
    float X;
    char __paddingB[12];
    float Y;
    char __paddingC[12];
    float Z;
};
