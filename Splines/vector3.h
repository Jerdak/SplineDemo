#ifndef __vector3_h__
#define __vector3_h__

class vector3
{
	public:
		vector3() : x( 0.0f ), y( 0.0f ), z( 0.0f ) {};
		vector3( float a_X, float a_Y, float a_Z ) : x( a_X ), y( a_Y ), z( a_Z ) {};
		vector3( const vector3 &v ) : x( v.x ), y( v.y ), z( v.z ) {};

		inline void Set( float a_X, float a_Y, float a_Z ) { x = a_X; y = a_Y; z = a_Z; }
		inline void Normalize() { float l = 1.0f / Length(); x *= l; y *= l; z *= l; }
		inline void Invert(){x*=-1;y*=-1;z*=-1;}

		inline bool IsZeroLength()				const { double d = x*x + y*y + z*z; return(d < (1e-06 * 1e-06));}
		inline float Dot(const vector3& a_V )	const { return x * a_V.x + y * a_V.y + z * a_V.z; }
		inline vector3 Cross( vector3 b )		const { return vector3( y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x ); }
		inline float Length()					const { return (float)sqrt( x * x + y * y + z * z ); }
		inline float SqrLength()					const { return x * x + y * y + z * z; }
		inline float Dist(const vector3 &a) const {
			float tmpX = (a.x - x) * (a.x - x);
			float tmpY = (a.y - y) * (a.y - y);
			float tmpZ = (a.z - z) * (a.z - z); 
			return sqrt(tmpX+tmpY+tmpZ);
		}
		inline float operator[](const int i) const { 
			return *(&x+i);
		}
		inline float& operator[](const int i)  { 
			return *(&x+i);
		}
		inline void operator += ( vector3& a_V ) { x += a_V.x; y += a_V.y; z += a_V.z; }
		inline void operator += ( int &val ) { x += val; y += val; z += val; }
		inline void operator += ( vector3* a_V ) { x += a_V->x; y += a_V->y; z += a_V->z; }
		inline void operator -= ( vector3& a_V ) { x -= a_V.x; y -= a_V.y; z -= a_V.z; }
		inline void operator -= ( vector3* a_V ) { x -= a_V->x; y -= a_V->y; z -= a_V->z; }
		inline void operator *= ( float f ) { x *= f; y *= f; z *= f; }
		inline void operator /= ( float f ) { x /= f; y /= f; z /= f; }
		inline void operator /= ( vector3& a_V ) { x /= a_V.x; y /= a_V.y; z /= a_V.z; }
		inline void operator *= ( vector3& a_V ) { x *= a_V.x; y *= a_V.y; z *= a_V.z; }
		inline void operator *= ( vector3* a_V ) { x *= a_V->x; y *= a_V->y; z *= a_V->z; }
		inline void operator = (const vector3 *a_v) {x = a_v->x; y = a_v->y; z = a_v->z;}
		inline bool operator ==(const vector3 &v)const {return (v.x == x && v.y == y && v.z == z);}
		inline bool operator !=(const vector3 &v)const {return !(v.x == x && v.y == y && v.z == z);}

		inline vector3 operator- () const { return vector3( -x, -y, -z ); }
		inline friend vector3 operator + ( const vector3& v1, const vector3& v2 ) { return vector3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z ); }
		inline friend vector3 operator - ( const vector3& v1, const vector3& v2 ) { return vector3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z ); }
		inline friend vector3 operator + ( const vector3& v1, vector3* v2 ) { return vector3( v1.x + v2->x, v1.y + v2->y, v1.z + v2->z ); }
		inline friend vector3 operator - ( const vector3& v1, vector3* v2 ) { return vector3( v1.x - v2->x, v1.y - v2->y, v1.z - v2->z ); }
		inline friend vector3 operator * ( const vector3& v, float f ) { return vector3( v.x * f, v.y * f, v.z * f ); }
		inline friend vector3 operator * ( const vector3& v1, vector3& v2 ) { return vector3( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z ); }
		inline friend vector3 operator * ( float f, const vector3& v ) { return vector3( v.x * f, v.y * f, v.z * f ); }
		inline friend vector3 operator / ( float f, const vector3& v ) { return vector3( v.x / f, v.y / f, v.z / f ); }
		inline friend vector3 operator / ( const vector3& v,float f ) { return vector3( v.x / f, v.y / f, v.z / f ); }
		inline friend vector3 operator / ( const vector3& v1, const vector3& v2) { return vector3( v1.x / v2.x, v1.y / v2.y, v1.z / v2.z ); }
		
		static vector3 vector3::Max(const vector3 &a, const vector3 &b) {
			vector3 max;
			if(a.x > b.x)max.x = a.x;
			else max.x = b.x;
			
			if(a.y > b.y)max.y = a.y;
			else max.y = b.y;
			
			if(a.z > b.z)max.z = a.z;
			else max.z = b.z;

			return max;
		}
		static vector3 vector3::Min(const vector3 &a, const vector3 &b) {
			vector3 min;
			if(a.x < b.x)min.x = a.x;
			else min.x = b.x;
			
			if(a.y < b.y)min.y = a.y;
			else min.y = b.y;
			
			if(a.z < b.z)min.z = a.z;
			else min.z = b.z;

			return min;
		}

		std::string toString(){
			char buffer[256];
			sprintf(buffer,"%f, %f, %f",x,y,z);
			return buffer;
		}
		union
		{
			struct { float x, y, z; };
			struct { float cell[3]; };
		};
};

#endif //__vector3_h__