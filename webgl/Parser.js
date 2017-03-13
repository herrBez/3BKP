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
	var L = [];
	var U = [];
	for(var i = 0; i < rawLines.length; i++){
		if(rawLines[i].trim().length > 0){
			var firstChar = rawLines[i].trim().charAt(0);
			if(firstChar != '#' && firstChar != 'L' && firstChar != 'U'){
				lines.push(rawLines[i].trim());
				counter++;
			} 
			var token = rawLines[i].split(/\s+/);
			if(firstChar == 'L'){
				L = [parseFloat(token[1]), parseFloat(token[2]), parseFloat(token[3])];
			}
			if(firstChar == 'U'){
				U = [parseFloat(token[1]), parseFloat(token[2]), parseFloat(token[3])];
			}
		}
	}
	
	console.log(lines.length);
	console.log("There are " + (counter-1) + " lines to parse");
	
	var position = [];
	var dimension = [];
	var itemIndices = [];
	
	var token = lines[0].split(/\s+/);
	var BoxDimension = new Array();
	var knapsack = -1;
	for(var i =0; i < lines.length; i++){
		
		console.log("line" + lines[i]);
		var token = lines[i].split(/\s+/);
		if(token.length == 3) {
			BoxDimension.push([parseFloat(token[0]), parseFloat(token[1]), parseFloat(token[2])]);
			knapsack = BoxDimension.length - 1;
			itemIndices.push(new Array());
			position.push(new Array());
			dimension.push(new Array());
		} else if(token.length == 7) {
			itemIndices[itemIndices.length-1].push(parseInt(token[0]));
			position[position.length-1].push([parseFloat(token[1]), parseFloat(token[2]), parseFloat(token[3])]);
			dimension[dimension.length-1].push([parseFloat(token[4]), parseFloat(token[5]), parseFloat(token[6])]);
		} else {
			alert("Parse Error: Number of token not recognized");
		}
		
		
	}
	calculateVertices(BoxDimension, itemIndices, position, dimension, L, U);
}

function calculateVertices(BoxDimension, itemIndices, position, dimension, L, U){
	var vertices = new Array();
	var centerOfMass = new Array();
	var BoxVertices = new Array();
	for(var k = 0; k < position.length; k++){
		var verticesTmp = new Array();
		var centerOfMassTmp = new Array();
		
		for(var i = 0; i < position[k].length; i++){
			var x = position[k][i][0];
			var y = position[k][i][1];
			var z = position[k][i][2];
		
			var dx = dimension[k][i][0];
			var dy = dimension[k][i][1];
			var dz = dimension[k][i][2];
		
		
			verticesTmp.push([
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
		
			centerOfMassTmp.push(
				[x+(dx/2.0), y+(dy/2.0), z+(dz/2.0)]
			);
		}
		vertices.push(verticesTmp);
		centerOfMass.push(centerOfMassTmp);
	}
	
	for(var k = 0; k < BoxDimension.length; k++){
		var x = 0;
		var y = 0;
		var z = 0;
		
		var dx = BoxDimension[k][0];
		var dy = BoxDimension[k][1];
		var dz = BoxDimension[k][2];
	
		BoxVertices.push([ 
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
	 }
	var innerBox = new Array();
	/*if(L.length == 3 && U.length == 3){
		innerBox = [
			//Back Face
			L[0], L[1], L[2],
			U[0], L[1], L[2],
			U[0], U[1], L[2],
			L[0], U[1], L[2],
			//Front Face
			L[0], L[1], U[2],
			U[0], L[1], U[2],
			U[0], U[1], U[2],
			L[0], U[1], U[2],	
		];
	}*/
	

	start(BoxVertices, itemIndices, vertices, centerOfMass, innerBox);
}

