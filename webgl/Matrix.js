function MultiplyMatrix(A, B){
	
	
	var m1A = A.elements;
	var m2A = B.elements;
	
	var m1 = [
		m1A[0][0], m1A[0][1], m1A[0][2], m1A[0][3],
		m1A[1][0], m1A[1][1], m1A[1][2], m1A[1][3],
		m1A[2][0], m1A[2][1], m1A[2][2], m1A[2][3],
		m1A[3][0], m1A[3][1], m1A[3][2], m1A[3][3],
		
	];
	var m2 = [
		m2A[0][0], m2A[0][1], m2A[0][2], m2A[0][3],
		m2A[1][0], m2A[1][1], m2A[1][2], m2A[1][3],
		m2A[2][0], m2A[2][1], m2A[2][2], m2A[2][3],
		m2A[3][0], m2A[3][1], m2A[3][2], m2A[3][3],
		
	];
	
	var temp = [
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
	];
    temp[0] = m1[0]*m2[0] + m1[1]*m2[4] + m1[2]*m2[8] + m1[3]*m2[12];
    temp[1] = m1[0]*m2[1] + m1[1]*m2[5] + m1[2]*m2[9] + m1[3]*m2[13];
    temp[2] = m1[0]*m2[2] + m1[1]*m2[6] + m1[2]*m2[10] + m1[3]*m2[14];
    temp[3] = m1[0]*m2[3] + m1[1]*m2[7] + m1[2]*m2[11] + m1[3]*m2[15];

    temp[4] = m1[4]*m2[0] + m1[5]*m2[4] + m1[6]*m2[8] + m1[7]*m2[12];
    temp[5] = m1[4]*m2[1] + m1[5]*m2[5] + m1[6]*m2[9] + m1[7]*m2[13];
    temp[6] = m1[4]*m2[2] + m1[5]*m2[6] + m1[6]*m2[10] + m1[7]*m2[14];
    temp[7] = m1[4]*m2[3] + m1[5]*m2[7] + m1[6]*m2[11] + m1[7]*m2[15];

    temp[8] = m1[8]*m2[0] + m1[9]*m2[4] + m1[10]*m2[8] + m1[11]*m2[12];
    temp[9] = m1[8]*m2[1] + m1[9]*m2[5] + m1[10]*m2[9] + m1[11]*m2[13];
    temp[10] = m1[8]*m2[2] + m1[9]*m2[6] + m1[10]*m2[10] + m1[11]*m2[14];
    temp[11] = m1[8]*m2[3] + m1[9]*m2[7] + m1[10]*m2[11] + m1[11]*m2[15];

    temp[12] = m1[12]*m2[0] + m1[13]*m2[4] + m1[14]*m2[8] + m1[15]*m2[12];
    temp[13] = m1[12]*m2[1] + m1[13]*m2[5] + m1[14]*m2[9] + m1[15]*m2[13];
    temp[14] = m1[12]*m2[2] + m1[13]*m2[6] + m1[14]*m2[10] + m1[15]*m2[14];
    temp[15] = m1[12]*m2[3] + m1[13]*m2[7] + m1[14]*m2[11] + m1[15]*m2[15];
	
	
	return $M([
	            [temp[0], temp[1], temp[2], temp[3]],
				[temp[4], temp[5], temp[6], temp[7]],
				[temp[8], temp[9], temp[10], temp[11]],
				[temp[12], temp[13], temp[14], temp[15]],
			  ]);
}
	


function getRotationX(angle)
{
	anglex = Math.PI/180 * angle;   /* Conversion angle from degree to radians */
    return $M([[1.0, 0.0, 0.0, 0.0],
			   [0.0, Math.cos(anglex), -Math.sin(anglex), 0.0],
			   [0.0, Math.sin(anglex), Math.cos(anglex), 0.0],
			   [0, 0,  0.0, 1.0],
			   ]);
}

function getRotationY(angle)
{
	
    anglex = Math.PI/180 * angle;   /* Conversion angle from degree to radians */
    return $M([[Math.cos(anglex),  0.0, Math.sin(anglex), 0.0],
			   [0.0, 1, 0.0, 0.0],
			   [-Math.sin(anglex), 0,  Math.cos(anglex), 0.0],
			   [0.0, 0.0, 0.0, 1.0]]);
}

function getRotationZ(angle)
{
	anglex = Math.PI/180 * angle;   /* Conversion angle from degree to radians */
    return $M([[Math.cos(anglex),  -Math.sin(anglex), 0.0, 0.0],
			   [Math.sin(anglex), Math.cos(anglex), 0.0, 0.0],
			   [0, 0,  1.0, 0.0],
			   [0.0, 0.0, 0.0, 1.0]]);
}
