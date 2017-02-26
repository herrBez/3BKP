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
	console.log("There are " + counter + " objects in the knapsack");
	
	var position = new Array();
	var dimension = new Array();
	
	for(var i =0; i < lines.length; i++){
		console.log(lines[i]);
		var token = lines[i].split(/\s+/);
		var index = parseInt(token[0]);
		position.push([parseFloat(token[1]), parseFloat(token[2]), parseFloat(token[3])]);
		dimension.push([parseFloat(token[4]), parseFloat(token[5]), parseFloat(token[6])]);
	}
	calculateVertices(position, dimension);
}

function calculateVertices(position, dimension){
	var vertices = new Array();
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
	}
	start(vertices);
}

