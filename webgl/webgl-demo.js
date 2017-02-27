var canvas;
var gl;
var squareVerticesBuffer;

var cubeVerticesBuffer;
var cubeVerticesIndexBuffer;
var cubeVerticesColor;


var VerticesBuffer;
var ColorBuffer;



var ModelMatrix;
var ViewMatrix;

var shaderProgram;

var vertexPositionAttribute;
var vertexColorAttribute;

var perspectiveMatrix;


var X = 0;
var Y = 1;
var Z = 2;


var vertices;
var boxVertices;

var CAMERA_STANDARD_Z = -20;

var move_camera = false;
var camerapos = [0, 0, CAMERA_STANDARD_Z];

var rotationEnabled = [false, false, false];
var rotationCoefficient = [1, 1, 1];


var colorInformation;

function initMatrix(){
	camerapos[Z] = CAMERA_STANDARD_Z;
	rotationEnabled = [false,false,false];
	ViewMatrix = Matrix.I(4);
	ViewMatrix.elements[0][3] = camerapos[X];
	ViewMatrix.elements[1][3] = camerapos[Y];
	ViewMatrix.elements[2][3] = camerapos[Z];
		
	ModelMatrix = Matrix.I(4);
	ModelMatrix.elements[0][0] = 1.0;
	ModelMatrix.elements[1][1] = 1.0;
	ModelMatrix.elements[2][2] = 1.0;
	ModelMatrix.elements[0][3] = 0.0;
	ModelMatrix.elements[1][3] = 0.0;
	ModelMatrix.elements[2][3] = 0.0;
}

//
// start
//
// Called when the canvas is created to get the ball rolling.
// Figuratively, that is. There's nothing moving in this demo.
//
function start(_boxVertices, _vertices) {
  console.log("Start");
  boxVertices = _boxVertices;
  vertices = _vertices; 	
	
  canvas = document.getElementById("glcanvas");
  
  canvas.addEventListener('click', function(evt){
		move_camera = !move_camera;
			},false);

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

  
  colorInformation = new Array();
  for(var i = 0; i < vertices.length; i++){
	 var r = Math.random();
	 var g = Math.random();
	 var b = Math.random();
	 colorInformation.push(
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
  var colorBuffer = [
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
	for(var i = 0; i < vertices.length; i++){
		console.log("Binding " + vertices[i] );
		VerticesBuffer.push(gl.createBuffer());
		gl.bindBuffer(gl.ARRAY_BUFFER, VerticesBuffer[i]);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices[i]), gl.STATIC_DRAW);
		
		ColorBuffer.push(gl.createBuffer());
		gl.bindBuffer(gl.ARRAY_BUFFER, ColorBuffer[i]);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colorInformation[i]), gl.STATIC_DRAW);
	}
	
	
	
	cubeVerticesBuffer = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(boxVertices), gl.STATIC_DRAW);

	cubeVerticesColor = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesColor);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colorBuffer), gl.STATIC_DRAW);

	//cubeVerticesIndexBuffer = gl.createBuffer();
	//gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer);
	//gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(cubeVertexIndices), gl.STATIC_DRAW);
	cubeVerticesIndexBuffer = new Array();
	cubeVerticesIndexBuffer.push(gl.createBuffer());
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer[0]);
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(cubeVertexIndicesTriangles), gl.STATIC_DRAW);
	
	cubeVerticesIndexBuffer.push(gl.createBuffer());
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer[1]);
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(cubeVertexIndicesLines), gl.STATIC_DRAW);
}

function rotation(){
	
	if(rotationEnabled[0]){
		var RotationMatrixX = getRotationX(0.2*rotationCoefficient[0]);
		ViewMatrix = MultiplyMatrix(ViewMatrix, RotationMatrixX);
	}
	if(rotationEnabled[1]){
		var RotationMatrixY = getRotationY(0.2*rotationCoefficient[1]);
		ViewMatrix = MultiplyMatrix(ViewMatrix, RotationMatrixY);
	}
	if(rotationEnabled[2]){
		var RotationMatrixZ = getRotationZ(0.2*rotationCoefficient[2]);
		ViewMatrix = MultiplyMatrix(ViewMatrix, RotationMatrixZ);
	}
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

  perspectiveMatrix = makePerspective(45, 640.0/480.0, 0.1, 100.0);

  // Set the drawing position to the "identity" point, which is
  // the center of the scene.

  
	
  // Draw the square by binding the array buffer to the square's vertices
  // array, setting attributes, and pushing it to GL.
		
	ViewMatrix.elements[0][3] = camerapos[X];
	ViewMatrix.elements[1][3] = camerapos[Y];
	ViewMatrix.elements[2][3] = camerapos[Z];
	rotation();
	setMatrixUniforms();
	
	for(var i = 0; i < vertices.length; i++){
		gl.bindBuffer(gl.ARRAY_BUFFER, ColorBuffer[i]);
		gl.vertexAttribPointer(vertexColorAttribute, 3, gl.FLOAT, false, 0, 0);
	
		gl.bindBuffer(gl.ARRAY_BUFFER, VerticesBuffer[i]);
		gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
	
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer[0]);
		gl.drawElements(gl.TRIANGLES, 36, gl.UNSIGNED_SHORT, 0);
	}
	
	gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesColor);
	gl.vertexAttribPointer(vertexColorAttribute, 3, gl.FLOAT, false, 0, 0);

	gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesBuffer);
	gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer[1]);
	gl.drawElements(gl.LINES, 24, gl.UNSIGNED_SHORT, 0);
	
	
  
 
  
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

function setMatrixUniforms() {
  var pUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
  gl.uniformMatrix4fv(pUniform, false, new Float32Array(perspectiveMatrix.flatten()));

  var modelUniform = gl.getUniformLocation(shaderProgram, "ModelMatrix");
  gl.uniformMatrix4fv(modelUniform, false, new Float32Array(ModelMatrix.flatten()));
  
  var viewUniform =  gl.getUniformLocation(shaderProgram, "ViewMatrix");
  gl.uniformMatrix4fv(viewUniform, false, new Float32Array(ViewMatrix.flatten()));
}


function KeyBoard(code){
	switch(code){
		case "X".charCodeAt(0): rotationEnabled[0] = !rotationEnabled[0]; break; 
		case "Y".charCodeAt(0): rotationEnabled[1] = !rotationEnabled[1]; break;
		case "Z".charCodeAt(0): rotationEnabled[2] = !rotationEnabled[2]; break;
		case 187: //+ in CHROME
		case 171: camerapos[Z]++; break; //+ in Firefox
		case 189:
		case 173: camerapos[Z]--; break; //-
		
		case 38: camerapos[Y]++; break; //KEYUP
		case 40: camerapos[Y]--; break; //KEYDOWN
		case 39: camerapos[X]--; break; //KEYLEFT
		case 37: camerapos[X]++; break; //KEYRIGHT
		case "R".charCodeAt(0): initMatrix(); break; //RESET
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
	"R -> Reset</br>";
	

