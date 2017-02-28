function handleFileSelect(evt) {
	var files = evt.target.files; // FileList object
	var output = [];
	var f = files[0];
	console.log(f.name);
	var reader = new FileReader();
	reader.onload = function(e){
		
		var text = reader.result;
		processText(text);
		
	}
	reader.readAsText(f);
}


function processText(text){
	
	
	var rawLines = text.split("\n");
	var lines = new Array();
	var counter = 0;
	for(var i = 0; i < rawLines.length; i++){
		if(rawLines[i].trim().length > 0){
			if(rawLines[i].trim().charAt(0) != '#'){
				lines.push(rawLines[i].trim());
				counter++;
			}
		}
	}
	console.log(lines.length);
	console.log("There are " + (counter-1) + " objects in the knapsack");
	
	var position = new Array();
	var dimension = new Array();
	var itemIndices = new Array();
	
	var token = lines[0].split(/\s+/);
	var BoxDimension = [parseFloat(token[0]), parseFloat(token[1]), parseFloat(token[2])];
	
	for(var i =1; i < lines.length; i++){
		console.log(lines[i]);
		var token = lines[i].split(/\s+/);
		itemIndices.push(parseInt(token[0]));
		position.push([parseFloat(token[1]), parseFloat(token[2]), parseFloat(token[3])]);
		dimension.push([parseFloat(token[4]), parseFloat(token[5]), parseFloat(token[6])]);
	}
	calculateVertices(BoxDimension, itemIndices, position, dimension);
}

function calculateVertices(BoxDimension, itemIndices, position, dimension){
	var vertices = new Array();
	var centerOfMass = new Array();
	for(var i = 0; i < position.length; i++){
		var x = position[i][0];
		var y = position[i][1];
		var z = position[i][2];
		
		var dx = dimension[i][0];
		var dy = dimension[i][1];
		var dz = dimension[i][2];
		
		
		vertices.push([
			//Back Face
			x, 		y, 		z,
			x+dx, 	y, 		z,
			x+dx,	y+dy, 	z,
			x, 		y+dy,	z,
			//Front Face
			x, 		y, 		z+dz,
			x+dx, 	y, 		z+dz,
			x+dx,	y+dy, 	z+dz,
			x, 		y+dy,	z+dz,
		]);
		centerOfMass.push(
			x+(dx/2.0), y+(dy/2.0), z+(dz/2.0)	
		);
	}
	var x = 0;
	var y = 0;
	var z = 0;
	var dx = BoxDimension[0];
	var dy = BoxDimension[1];
	var dz = BoxDimension[2];
	
	BoxVertices = [ 
			//Back Face
			x, 		y, 		z,
			x+dx, 	y, 		z,
			x+dx,	y+dy, 	z,
			x, 		y+dy,	z,
			//Front Face
			x, 		y, 		z+dz,
			x+dx, 	y, 		z+dz,
			x+dx,	y+dy, 	z+dz,
			x, 		y+dy,	z+dz,
		
	];
	
	//alert(BoxVertices);
	start(BoxVertices, itemIndices, vertices, centerOfMass);
}

