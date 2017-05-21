# R programs
args <- commandArgs(TRUE)
printf <- function(...) invisible(print(sprintf(...)))


mydata = read.csv(args[1], header=TRUE, sep=",")
printf("%f", mean(mydata$CPU.Time), mean(mydata$User.Time))
printf("%f", max(mydata$CPU.Time), max(mydata$User.Time))
printf("%f", min(mydata$CPU.Time), min(mydata$User.Time))
