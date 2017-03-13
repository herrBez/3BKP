/**
 * author: Mirko Bez
 * 
 */
var canvas;
var gl;
var squareVerticesBuffer;

var cubeVerticesBuffer;
var cubeVerticesIndexBuffer;
var cubeVerticesColor;


var VerticesBuffer;
var ColorBuffer;

var CenterOfMassBuffer;

var ModelMatrix;
var ViewMatrix;

var shaderProgram;

var vertexPositionAttribute;
var vertexColorAttribute;

var perspectiveMatrix;

var centerOfMass;

var X = 0;
var Y = 1;
var Z = 2;

var LINES = 0;
var TRIANGLE = 1;

var extended;

var vertices;
var boxVertices;
var innerBoxVertices;
var innerBoxVerticesBuffer;
var camerapos = [0, 0, 0];

var rotationEnabled = [false, false, false];
var rotationCoefficient = [1, 1, 1];



var representAsTriangle;


var zoom_step = [0, 0, 0];

var padding = 0.2;

var enabledCenterOfMass = false;

function findMaxZ() {
	var max = boxVertices[0][3*4 + Z];
	for(var k = 1; k < boxVertices.length; k++){
		if(max < boxVertices[k][3*4 + Z])
			max = boxVertices[k][3*4 + Z];
	} 
	return max;
}

function findCenter() {
	var l = 0;
	for(var k = 0; k < boxVertices.length; k++){
		l += (boxVertices[k][3*7 + X] - boxVertices[k][3*6 + X])
		l += padding;
	}
	
	return l/2;
}

function initMatrix(){
	
	camerapos[X] = findCenter();
	camerapos[Y] = (-boxVertices[0][3*7 + Y] + boxVertices[0][3*4 + Y])/2;
	camerapos[Z] = -6*findMaxZ();

	
	zoom_step[X] = boxVertices[0][3*6 + X] / 20;
	zoom_step[Y] = boxVertices[0][3*6 + Y] / 20;
	zoom_step[Z] = boxVertices[0][3*6 + Z] / 20;
	
	
	rotationEnabled = [false,false,false];
	ViewMatrix = Matrix.I(4);
	ViewMatrix.elements[0][3] = camerapos[X];
	ViewMatrix.elements[1][3] = camerapos[Y];
	ViewMatrix.elements[2][3] = camerapos[Z];
		
	ModelMatrix = new Array();
	for(k = 0; k < boxVertices.length; k++){
		ModelMatrix.push(Matrix.I(4));
		var x = 0;
		for(l = 0; l < k; l++){
			x += (-(boxVertices[l][3*7 + X] - boxVertices[l][3*6 + X]));
			x += padding;
		}
		ModelMatrix[k].elements[0][3] = x; //Move in x axis
		ModelMatrix[k].elements[1][3] = 0.0; //Move in y axis
		ModelMatrix[k].elements[2][3] = 0.0; //Move in z axis
		console.log("THIS " + ModelMatrix[k].elements[0][3]);
	}
}

function initAttributes(){
	representAsTriangle = new Array();
	var i = 0;
	var k = 0;
	for(k = 0; k < boxVertices.length; k++){
		var tmp = new Array();
		for(i = 0; i < vertices[k].length; i++){
			tmp.push(true);
		}
		representAsTriangle.push(tmp);
	}
	
	
}

//
// start
//
// Called when the canvas is created to get the ball rolling.
// Figuratively, that is. There's nothing moving in this demo.
//
function start(_boxVertices, _itemIndices, _vertices, _centerOfMass, _innerBoxVertices) {
  console.log("Start");
  boxVertices = _boxVertices;
  vertices = _vertices; 	
  centerOfMass = _centerOfMass;
  innerBoxVertices = _innerBoxVertices;
  
  initAttributes();
	//addSelector();
  canvas = document.getElementById("glcanvas");
  
  canvas.addEventListener('click', function(evt){},false);

  initWebGL(canvas);      // Initialize the GL context

  // Only continue if WebGL is available and working

  if (gl) {
    gl.clearColor(0.0, 0.0, 0.0, 1.0);  // Clear to black, fully opaque
    gl.clearDepth(1.0);                 // Clear everything
    gl.enable(gl.DEPTH_TEST);           // Enable depth testing
    gl.depthFunc(gl.LEQUAL);            // Near things obscure far things

    // Initialize the shaders; this is where all the lighting for the
    // vertices and so forth is established.

    initShaders();

    
    initMatrix();
    // Here's where we call the routine that builds all the objects
    // we'll be drawing.

    
    initBuffers();

    // Set up to draw the scene periodically.

    setInterval(drawScene, 15);
  }
}

//
// initWebGL
//
// Initialize WebGL, returning the GL context or null if
// WebGL isn't available or could not be initialized.
//
function initWebGL() {
  gl = null;

  try {
    gl = canvas.getContext("experimental-webgl");
  }
  catch(e) {
  }

  // If we don't have a GL context, give up now

  if (!gl) {
    alert("Unable to initialize WebGL. Your browser may not support it.");
  }
}

//
// initBuffers
//
// Initialize the buffers we'll need. For this demo, we just have
// one object -- a simple two-dimensional square.
//
function initBuffers() {
  
  var colorInformation = new Array();
  /* Assign random colors to the different polygons */
  for(var k = 0; k < boxVertices.length; k++){
	  var colorInformationTmp = new Array();
		for(var i = 0; i < vertices[k].length; i++){
			var r = Math.random();
			var g = Math.random();
			var b = Math.random();
			colorInformationTmp.push(
				[
					r, g, b,
					r, g, b,
					r, g, b,
					r, g, b,
					r, g, b,
					r, g, b,
					r, g, b,
					r, g, b,
				]
			);
		}
		colorInformation.push(colorInformationTmp);
	}
  var colorCubeVertices = [
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	];
	
	
  
  var cubeVertexIndicesLines = [
	0, 1, 1, 2, 2, 3, 3, 0, //Back square
	4, 5, 5, 6, 6, 7, 7, 4, //Front square
	0, 4, 1, 5, 2, 6, 3, 7, //Bind them
  ];
	
  
  var cubeVertexIndicesTriangles = [
    0, 1, 2,
    2, 3, 0,
    1, 5, 6,
    6, 2, 1,
    7, 6, 5,
    5, 4, 7,
    4, 0, 3,
    3, 7, 4,
    4, 5, 1,
    1, 0, 4,
    3, 2, 6,
    6, 7, 3,
  ];
  
  VerticesBuffer = new Array();
  ColorBuffer = new Array();
  // Now pass the list of vertices into WebGL to build the shape. We
  // do this by creating a Float32Array from the JavaScript array,
  // then use it to fill the current vertex buffer.
  
	for(var k = 0; k < boxVertices.length; k++){
		VerticesBufferTmp = new Array();
		ColorBufferTmp = new Array();
		
		for(var i = 0; i < vertices[k].length; i++){
		
			VerticesBufferTmp.push(gl.createBuffer());
			gl.bindBuffer(gl.ARRAY_BUFFER, VerticesBufferTmp[i]);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices[k][i]), gl.STATIC_DRAW);

			ColorBufferTmp.push(gl.createBuffer());
			gl.bindBuffer(gl.ARRAY_BUFFER, ColorBufferTmp[i]);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colorInformation[k][i]), gl.STATIC_DRAW);
		}
		VerticesBuffer.push(VerticesBufferTmp);
		ColorBuffer.push(ColorBufferTmp);
	}
	
	centerOfMassBuffer = new Array();
	for(var k = 0; k < boxVertices.length; k++){
		centerOfMassBufferTmp = new Array();
		for(var i = 0; i < centerOfMass[k].length; i++){
			centerOfMassBufferTmp.push(gl.createBuffer());
			gl.bindBuffer(gl.ARRAY_BUFFER, centerOfMassBufferTmp[i]);
			gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(centerOfMass[k][i]), gl.STATIC_DRAW);
		} 
		centerOfMassBuffer.push(centerOfMassBufferTmp);
	}
	
	
	/*if(extended){
		innerBoxVerticesBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, innerBoxVerticesBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(innerBoxVertices), gl.STATIC_DRAW);
	}*/
	cubeVerticesBuffer = new Array();
	for(var k = 0; k < boxVertices.length; k++){
		cubeVerticesBuffer.push(gl.createBuffer());
		gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesBuffer[k]);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(boxVertices[k]), gl.STATIC_DRAW);
	}
	
	cubeVerticesColor = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesColor);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colorCubeVertices), gl.STATIC_DRAW);

	
	cubeVerticesIndexBuffer = new Array();
	cubeVerticesIndexBuffer.push(gl.createBuffer());
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer[0]);
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(cubeVertexIndicesTriangles), gl.STATIC_DRAW);
	
	cubeVerticesIndexBuffer.push(gl.createBuffer());
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer[1]);
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(cubeVertexIndicesLines), gl.STATIC_DRAW);
}

function rotation(){
	
	if(rotationEnabled[X]){
		var RotationMatrixX = getRotationX(0.2*rotationCoefficient[X]);
		ViewMatrix = MultiplyMatrix(ViewMatrix, RotationMatrixX);
	}
	if(rotationEnabled[Y]){
		var RotationMatrixY = getRotationY(0.2*rotationCoefficient[Y]);
		ViewMatrix = MultiplyMatrix(ViewMatrix, RotationMatrixY);
	}
	if(rotationEnabled[Z]){
		var RotationMatrixZ = getRotationZ(0.2*rotationCoefficient[Z]);
		ViewMatrix = MultiplyMatrix(ViewMatrix, RotationMatrixZ);
	}
}


function drawParallelepiped(vbo, cbo, ibo, mode, numberOfElements){
	gl.bindBuffer(gl.ARRAY_BUFFER, cbo);
	gl.vertexAttribPointer(vertexColorAttribute, 3, gl.FLOAT, false, 0, 0);

	gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
	gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ibo);
	gl.drawElements(mode, numberOfElements, gl.UNSIGNED_SHORT, 0);  
}

function drawArray(vbo, cbo, mode, start_index, numberOfElements){
	gl.bindBuffer(gl.ARRAY_BUFFER, cbo);
	gl.vertexAttribPointer(vertexColorAttribute, 3, gl.FLOAT, false, 0, 0);
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
	gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
	gl.drawArrays(mode, start_index, numberOfElements);
}

//
// drawScene
//
// Draw the scene.
//
function drawScene() {
  // Clear the canvas before we start drawing on it.

  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  // Establish the perspective with which we want to view the
  // scene. Our field of view is 45 degrees, with a width/height
  // ratio of 640:480, and we only want to see objects between 0.1 units
  // and 100 units away from the camera.

  perspectiveMatrix = makePerspective(45, 640.0/480.0, 0.1, findMaxZ()*20);

  // Set the drawing position to the "identity" point, which is
  // the center of the scene.

  
	
  // Draw the square by binding the array buffer to the square's vertices
  // array, setting attributes, and pushing it to GL.
		
	ViewMatrix.elements[0][3] = camerapos[X];
	ViewMatrix.elements[1][3] = camerapos[Y];
	ViewMatrix.elements[2][3] = camerapos[Z];
	rotation();
	
		
	for(var k = 0; k < boxVertices.length; k++){
		setMatrixUniforms(ModelMatrix[k]);
		for(var i = 0; i < vertices[k].length; i++){
			if(!enabledCenterOfMass){
				if(representAsTriangle[k][i]){
					drawParallelepiped(VerticesBuffer[k][i], ColorBuffer[k][i], cubeVerticesIndexBuffer[0], gl.TRIANGLES, 36); 
				}
				else{	
					drawParallelepiped(VerticesBuffer[k][i], ColorBuffer[k][i], cubeVerticesIndexBuffer[1], gl.LINES, 24); 
				}
			}
		}
		if(enabledCenterOfMass){
			for(var i = 0; i < centerOfMass[k].length; i++){
				drawArray(centerOfMassBuffer[k][i], ColorBuffer[k][i], gl.POINTS, 0, 1);
				
			}
		}
	
	
		drawParallelepiped(cubeVerticesBuffer[k], cubeVerticesColor, cubeVerticesIndexBuffer[1], gl.LINES, 24);
	}
}

//
// initShaders
//
// Initialize the shaders, so WebGL knows how to light our scene.
//
function initShaders() {
  var fragmentShader = getShader(gl, "shader-fs");
  var vertexShader = getShader(gl, "shader-vs");
	
  // Create the shader program

  shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertexShader);
  gl.attachShader(shaderProgram, fragmentShader);
  gl.linkProgram(shaderProgram);
  // If creating the shader program failed, alert

  if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    alert("Unable to initialize the shader program: " + gl.getProgramInfoLog(shader));
  }

  gl.useProgram(shaderProgram);
 
  vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
  gl.enableVertexAttribArray(vertexPositionAttribute);
  
  vertexColorAttribute = gl.getAttribLocation(shaderProgram, "aColor");
  gl.enableVertexAttribArray(vertexColorAttribute);
}

//
// getShader
//
// Loads a shader program by scouring the current document,
// looking for a script with the specified ID.
//
function getShader(gl, id) {
  var shaderScript = document.getElementById(id);

  // Didn't find an element with the specified ID; abort.

  if (!shaderScript) {
    return null;
  }

  // Walk through the source element's children, building the
  // shader source string.

  var theSource = "";
  var currentChild = shaderScript.firstChild;

  while(currentChild) {
    if (currentChild.nodeType == 3) {
      theSource += currentChild.textContent;
    }

    currentChild = currentChild.nextSibling;
  }

  // Now figure out what type of shader script we have,
  // based on its MIME type.

  var shader;

  if (shaderScript.type == "x-shader/x-fragment") {
    shader = gl.createShader(gl.FRAGMENT_SHADER);
  } else if (shaderScript.type == "x-shader/x-vertex") {
    shader = gl.createShader(gl.VERTEX_SHADER);
  } else {
    return null;  // Unknown shader type
  }

  // Send the source to the shader object

  gl.shaderSource(shader, theSource);

  // Compile the shader program

  gl.compileShader(shader);

  // See if it compiled successfully

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    alert("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
    return null;
  }

  return shader;
}

//
// Matrix utility functions
//

function loadIdentity() {
  ModelMatrix = Matrix.I(4);
}

function multMatrix(m) {
  ModelMatrix = mvMatrix.x(m);
}

function mvTranslate(v) {
  multMatrix(Matrix.Translation($V([v[0], v[1], v[2]])).ensure4x4());
}

function setMatrixUniforms(SelectedModelMatrix) {
  var pUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
  gl.uniformMatrix4fv(pUniform, false, new Float32Array(perspectiveMatrix.flatten()));

  var modelUniform = gl.getUniformLocation(shaderProgram, "ModelMatrix");
  gl.uniformMatrix4fv(modelUniform, false, new Float32Array(SelectedModelMatrix.flatten()));
  
  var viewUniform =  gl.getUniformLocation(shaderProgram, "ViewMatrix");
  gl.uniformMatrix4fv(viewUniform, false, new Float32Array(ViewMatrix.flatten()));
}


function KeyBoard(code){
	switch(code){
		case "I".charCodeAt(0): rotationCoefficient[X] = -1*rotationCoefficient[X];
								rotationCoefficient[Y] = -1*rotationCoefficient[Y];
								rotationCoefficient[Z] = -1*rotationCoefficient[Z]; break;
		case "X".charCodeAt(0): rotationEnabled[0] = !rotationEnabled[0]; break; 
		case "Y".charCodeAt(0): rotationEnabled[1] = !rotationEnabled[1]; break;
		case "Z".charCodeAt(0): rotationEnabled[2] = !rotationEnabled[2]; break;
		case 187: //+ in CHROME
		case 171: camerapos[Z]+=zoom_step[Z]; break; //+ in Firefox
		case 189: 
		case 173: camerapos[Z]-=zoom_step[Z]; break; //-
		
		case 38: camerapos[Y]+= zoom_step[Y]; break; //KEYUP
		case 40: camerapos[Y]-= zoom_step[Y]; break; //KEYDOWN
		case 39: camerapos[X]-= zoom_step[X]; break; //KEYLEFT
		case 37: camerapos[X]+= zoom_step[X]; break; //KEYRIGHT
		case "T".charCodeAt(0): 
			for(var k = 0; k < boxVertices.length; k++){
				for(var i = 0; i < vertices[k].length; i++){
					representAsTriangle[k][i] = !representAsTriangle[k][i]; 
				}
			}
			break;
		case "B".charCodeAt(0):
			var rotation = getRotationY(180);
			ViewMatrix = MultiplyMatrix(ViewMatrix, rotation);
			break;
		case "R".charCodeAt(0): initMatrix(); 
								initAttributes(); 
								break; //RESET
		case "C".charCodeAt(0): enabledCenterOfMass = !enabledCenterOfMass; 
								break;
		default: console.log("Error: code " + code + " not recognized ");
	}
}

var description = ""+
	"X -> Rotation around X axis</br>"+
	"Y -> Rotation around Y axis</br>"+
	"Z -> Rotaiton around Z axis</br>"+
	"+ -> Zoom in</br>" +
	"- -> Zoom out</br>"+
	"up -> Move camera up</br>"+
	"down -> Move camera down</br>"+
	"right -> Move camera right</br>"+
	"left -> Move camera left</br>"+
	"R -> Reset</br>" + 
	"T -> draw parallelepipedes as lines and not solid or vice versa <br>" + 
	"I -> Invert the sense of rotation<br>" +
	"B -> 180 degree rotation around Y axis" + 
	"C -> Show Center of Mass";
	
	/*
function addSelector(){
	var selectDIV = document.getElementById("Select");
		
	var s = "<select id=\"selectId\">";
	for(var i = 0; i < vertices.length; i++){
		s += "<option value=\"" + i + '\">' + i + '</option>';
	}
	s += "</select>";
	
	selectDIV.innerHTML = s;
	var selectField = document.getElementById("selectId");
	
	selectField.addEventListener("change", function() {
		var i = parseInt(selectField.value);
		representAsTriangle[i] = !representAsTriangle[i];
   
});
}*/


