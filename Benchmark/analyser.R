# R programs
args <- commandArgs(TRUE)
printf <- function(...) invisible(print(sprintf(...)))

analyse <- function(mydata) {
	
	return(mean(mydata$CPU.Time));
}

if(length(args) < 2){
	stop("Rscript <name.R> <output_file> <directory> <regex>")
}

directory <- args[2]

regex <- args[3] #".main\\.all\\.csv$"


#mydata = read.csv(args[1], header=TRUE, sep=",")
filenames <- list.files(directory, pattern=regex, full.names=TRUE)

con <- file(args[1], "w")
cat(
	sprintf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
		"\\documentclass{scrartcl}",
		"\\begin{document}",
		"\\begin{table}[h!]",
		"\\begin{center}",
		"\\small",
		"\\begin{tabular}{| c | c | c | c | c | c | c | c | c | c |}",
		"\\hline",
		"inst. & done & AUT & CU & ACT & max & min & CV-T & ObjV & CV-O \\\\",
		"\\hline")
	,
	file=con)
		
	

ldf <- lapply(filenames, function(i){
	print(cat("READ", i, "\n"));
	mydata<-read.csv(i, header=TRUE, skip=",")
	if(length(mydata$CPU.Time) > 0){
		print(length(mydata$Timeout.Reached[mydata$Timeout.Reached == "Yes"]))
		print(length(mydata$Timeout.Reached['No']))
		cat(
			sprintf("\\verb|%s| & %d / %d & %.4f & %.4f & %.4f & %.4f & %.4f & %.4f & %.2f & %.2f\\\\ \n", 
					strsplit(basename(i), "\\.")[[1]][1],
					length(grep('No', mydata$Timeout.Reached)),
					length(mydata$Timeout.Reached),
					mean(mydata$User.Time)/60,
					mean(mydata$CPU.Time)/mean(mydata$User.Time),
					mean(mydata$CPU.Time)/60,
					max(mydata$CPU.Time)/60,
					min(mydata$CPU.Time)/60,
					(sd(mydata$CPU.Time)/mean(mydata$CPU.Time))*100,
					mean(mydata$Objval),
					(sd(mydata$Objval)/mean(mydata$Objval))*100
			),
			file=con
		)
	} else {
		
		cat(
			sprintf("\\verb|%s| & %d & %s & %s & %s & %s & %s & %s & \\\\ \n", 
					strsplit(basename(i), "\\.")[[1]][1],
					0,
					"NA",
					"NA",
					"NA",
					"NA",
					"NA",
					"NA",
					"NA"
					), 
			file=con
		)
	}
})

cat(sprintf("\\hline\n\\end{tabular}\n\\caption{:}\n\\label{table:}\n\\end{center}\n\\end{table}\n\\end{document}"), file = con);

close(con);


##result = lapply(ldf, analyse);

#EXIT if no data available
#if(length(mydata$CPU.time) < 1){
#	stop("No data available");
#}
#printf("%f", mean(mydata$CPU.Time), mean(mydata$User.Time))
#printf("%f", max(mydata$CPU.Time), max(mydata$User.Time))
#printf("%f", min(mydata$CPU.Time), min(mydata$User.Time))
