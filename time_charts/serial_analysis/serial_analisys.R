n <- 50

data <- list()

paths = list.files(path = "C:/Users/ricca/Desktop/Architetture parallele/Parallel-solution-for-finding-all-solutions-to-the-stable-marriage-problem/time_charts/serial_analysis", pattern = ".txt", full.names = TRUE)
temp = paths[8]
for (i in 8:2) {
  paths[i] = paths[i-1] 
}
paths[1] = temp
paths

#reading files and computing means for each test
means <- data.frame()
for (i in 1:8) {
  content <- read.table(paths[i], header=TRUE, sep=" ")
  data[[length(data)+1]] <- content
  
  r = c(mean(content$GaleShapley), mean(content$FindAllRotations), mean(content$BuildGraph), mean(content$RecursiveSearch))
  means <- rbind(means, r)
}

#columns renaming
colnames(means) <- c("gale_shapley", "find_all_rotations", "build_graph", "recursive_search")
means


#computing percentages
perc <- data.frame()
for (i in 1:8) {
  total <- sum(data[[i]]$Total)
  gale_shapley_perc <- (sum(data[[i]]$GaleShapley) / total) * 100
  find_all_rotations_perc <- (sum(data[[i]]$FindAllRotations) / total) * 100
  build_graph_perc <- (sum(data[[i]]$BuildGraph) / total) * 100
  recursive_search_perc <- (sum(data[[i]]$RecursiveSearch) / total) * 100
  total_perc <- gale_shapley_perc + find_all_rotations_perc + build_graph_perc + recursive_search_perc
  r <- c(gale_shapley_perc, find_all_rotations_perc, build_graph_perc, recursive_search_perc, total_perc)
  perc <- rbind(perc, r)
}

#columns renaming
colnames(perc) <- c("gale_shapley_perc", "find_all_rotations_perc", "build_graph_perc", "recursive_search_perc", "total_perc")
perc



#asse y %, asse x dimensione istanze, fill di sub_algorithm
#plot = ggplot(tst, aes(x=instance_dimension, y=time, fill=sub_algorithm))
#plot+
#  geom_area(colour="black", size=.2, alpha=.8) +
#  theme_bw()