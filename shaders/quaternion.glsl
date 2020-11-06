// See: https://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
vec4 quat_conj(vec4 q)
{ 
  return vec4(-q.x, -q.y, -q.z, q.w); 
}
  
vec4 quat_mult(vec4 q1, vec4 q2)
{ 
  vec4 qr;
  qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
  qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
  qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
  qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
  return qr;
}

vec4 rotate_vertex_position(vec4 q, vec4 position)
{ 
  vec4 q_conj = quat_conj(q);
  vec4 q_pos = vec4(position.x, position.y, position.z, 0);
  
  vec4 q_tmp = quat_mult(q, q_pos);
  vec4 r = quat_mult(q_tmp, q_conj);
  
  return vec4(r.xyz, position.w);
}
