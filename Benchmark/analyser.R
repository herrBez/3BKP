# R programs
args <- commandArgs(TRUE)
printf <- function(...) invisible(print(sprintf(...)))

analyse <- function(mydata) {
	
	return(mean(mydata$CPU.Time));
}

#mydata = read.csv(args[1], header=TRUE, sep=",")
filenames <- list.files("FifteenKnapsacks", pattern="\\.main\\.all\\.csv$", full.names=TRUE)

con <- file("filename.tex", "w")
cat(
	sprintf("%s \n %s \n %s \n %s \n %s\n %s\n %s \n %s \n",
		"\\documentclass{article}",
		"\\begin{document}",
		"\\begin{table}",
		"\\begin{center}",
		"\\begin{tabular}{| c | c | c | c | c | c | c |}",
		"\\hline",
		"instance & completed & avg cpu used & average & max & min & CV \\% \\\\",
		"\\hline")
	,
	file=con)
		
	

ldf <- lapply(filenames, function(i){
	mydata<-read.csv(i, header=TRUE, skip=",")
	if(length(mydata$CPU.Time) > 0){
		print(length(mydata$Timeout.Reached[mydata$Timeout.Reached == "Yes"]))
		print(length(mydata$Timeout.Reached['No']))
		cat(
			sprintf("\\verb|%s| & %d / %d & %.2f & %.4f & %.4f & %.4f & %.4f \\\\ \n", 
					basename(i),
					length(grep('No', mydata$Timeout.Reached)),
					length(mydata$Timeout.Reached),
					mean(mydata$CPU.Time)/mean(mydata$User.Time),
					mean(mydata$User.Time)/60,
					max(mydata$User.Time)/60,
					min(mydata$User.Time)/60,
					(sd(mydata$User.Time)/mean(mydata$User.Time))*100
					
			),
			file=con
		)
	} else {
		cat(
			sprintf("\\verb|%s| & %d & %s & %s & %s & %s & %s & %s  \\\\ \n", 
					basename(i),
					0,
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

cat(sprintf("\\hline\n\\end{tabular}\n\\end{center}\n\\end{table}\n\\end{document}"), file = con);

close(con);


##result = lapply(ldf, analyse);

#EXIT if no data available
#if(length(mydata$CPU.time) < 1){
#	stop("No data available");
#}
#printf("%f", mean(mydata$CPU.Time), mean(mydata$User.Time))
#printf("%f", max(mydata$CPU.Time), max(mydata$User.Time))
#printf("%f", min(mydata$CPU.Time), min(mydata$User.Time))
