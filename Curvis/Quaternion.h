#pragma once
#include <glm\glm.hpp>
typedef glm::mat4 mat4;
typedef glm::vec4 vec4;
typedef glm::vec3 vec3;
class Quaternion
{
public:
	Quaternion() { m_q = vec3(0.0f); m_s = 1.0f; }
	Quaternion(float s, vec3 v) { m_q = v; m_s = s; }
	Quaternion(const Quaternion& q) { m_q = q.m_q; m_s = q.m_s; }
	Quaternion& operator=(const Quaternion& q) { m_q = q.m_q; m_s = q.m_s; return *this; }
	~Quaternion() {}
	inline Quaternion operator +(const Quaternion& q)const {
		float s = m_s + q.m_s;
		vec3 iv = m_q + q.m_q;
		return Quaternion(s, iv);
	}
	inline Quaternion operator +=(const Quaternion& q) {
		m_s += q.m_s;
		m_q += q.m_q;
	}
	inline Quaternion operator -(const Quaternion& q)const {
		float s = m_s - q.m_s;
		vec3 iv = m_q - q.m_q;
		return Quaternion(s, iv);
	}
	inline Quaternion operator -=(const Quaternion& q){
		m_s -= q.m_s;
		m_q -= q.m_q;
	}
	inline Quaternion operator *(const Quaternion& q)const {
		float scalar = m_s * q.m_s - glm::dot(m_q,q.m_q);
		vec3 iv = q.m_q * m_s + m_q * q.m_s + glm::cross(m_q, q.m_q);
		return Quaternion(scalar, iv);
	}
	inline Quaternion operator *=(const Quaternion& q) { 
		float scalar = m_s * q.m_s - glm::dot(m_q, q.m_q);
		vec3 iv = q.m_q * m_s + m_q * q.m_s + glm::cross(m_q, q.m_q);
		(*this).m_q = iv;
		(*this).m_s = scalar;
	}
	inline Quaternion operator *(const float lm)const {
		float scalar = lm*m_s;
		vec3 iv = lm*m_q;
		return Quaternion(scalar, iv);
	}
	inline Quaternion operator *=(const float lm) {
		m_s = lm * m_s;
		m_q = lm * m_q;
	}
	float norm() {
		float s = m_s * m_s;
		float i = dot(m_q, m_q);
		return sqrt(s + i);
	}
	void normalize() {
		if (norm() != 0) {
			float l = 1 / norm();
			m_s *= l;
			m_q *= l;
		}
	}
	Quaternion conjugate() {
		float scalar = m_s;
		vec3 iv = -1.0f * m_q;
		return Quaternion(scalar, iv);
	}

	mat4 matrix() {

		float x = 2 * m_q.x * m_q.x;
		float y = 2 * m_q.y * m_q.y;
		float z = 2 * m_q.z * m_q.z;
		float xy = 2 * m_q.x * m_q.y;
		float xz = 2 * m_q.x * m_q.z;
		float zy = 2 * m_q.z * m_q.y;
		float xs = 2 * m_q.x * m_s;
		float ys = 2 * m_q.y * m_s;
		float zs = 2 * m_q.z * m_s;

		return mat4(1 - y - z, xy - zs, xz + ys, 0.0f,
			xy + zs, 1 - x - z, zy - xs, 0.0f,
			xz - ys, zy + xs, 1 - x - y, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}
	vec3 m_q;
	float m_s;
};