#COMPARAZIONE 1 - PARTE SERIALE
data <- list()

paths <- list.files(path = "C:/Users/ricca/Desktop/Architetture parallele/Parallel-solution-for-finding-all-solutions-to-the-stable-marriage-problem/time_charts/serial", pattern = ".txt", full.names = TRUE)
temp <- paths[9]
for (i in 9:2) {
  paths[i] <- paths[i-1] 
}
paths[1] <- temp
paths

#reading files and computing means for each test
means <- data.frame()
for (i in 1:10) {
  content <- read.table(paths[i], header=TRUE, sep=" ")
  data[[length(data)+1]] <- content
  mean <- c(mean(content$GaleShapley), 
            mean(content$FindAllRotations), 
            mean(content$BuildGraph), 
            mean(content$RecursiveSearch))
  means <- rbind(means, mean)
}
colnames(means) <- c("gale_shapley_mean", "find_all_rotations_mean", "build_graph_mean", "recursive_search_mean")
means

#preparing data for area plot
plot_frame1 <- data.frame()
instances_dim <- array()
for (i in 1:10) {
  instances_dim[i] <- i*500
}
sub_algorithm <- c("Gale-Shapley", "Find All Rotations", "Build Graph", "Recursive Search")
for (i in 1:10) {
  for (j in 1:4) {
    r <- c(instances_dim[i], sub_algorithm[j], means[i,j])
    plot_frame1 <- rbind(plot_frame1, r)
  }
}
colnames(plot_frame1) <- c("instances_dim", "sub_algorithm", "time")
plot_frame1$instances_dim <- as.integer(plot_frame1$instances_dim)
plot_frame1$time <- as.integer(plot_frame1$time)
plot_frame1


#COMPARAZIONE 1 - PARTE PARALLELA
data <- list()

paths <- list.files(path = "C:/Users/ricca/Desktop/Architetture parallele/Parallel-solution-for-finding-all-solutions-to-the-stable-marriage-problem/time_charts/parallel", pattern = ".txt", full.names = TRUE)
temp <- paths[9]
for (i in 9:2) {
  paths[i] <- paths[i-1] 
}
paths[1] <- temp
paths

#reading files and computing means for each test
means <- data.frame()
for (i in 1:10) {
  content <- read.table(paths[i], header=TRUE, sep=" ")
  content <- content[-c(1),]
  data[[length(data)+1]] <- content
  mean <- c(mean(content$GaleShapley), 
            mean(content$FindAllRotations), 
            mean(content$KernelOverhead), 
            mean(content$RecursiveSearch))
  means <- rbind(means, mean)
}
colnames(means) <- c("gale_shapley_mean", "find_all_rotations_mean", "build_graph_mean", "recursive_search_mean")
means

#preparing data for area plot
plot_frame2 <- data.frame()
instances_dim <- array()
for (i in 1:10) {
  instances_dim[i] <- i*500
}
sub_algorithm <- c("Gale-Shapley", "Find All Rotations", "Build Graph", "Recursive Search")
for (i in 1:10) {
  for (j in 1:4) {
    r <- c(instances_dim[i], sub_algorithm[j], means[i,j])
    plot_frame2 <- rbind(plot_frame2, r)
  }
}
colnames(plot_frame2) <- c("instances_dim", "sub_algorithm", "time")
plot_frame2$instances_dim <- as.integer(plot_frame2$instances_dim)
plot_frame2$time <- as.integer(plot_frame2$time)
plot_frame2

#plotting
library(ggplot2)
library(gridExtra)
plot1 <- ggplot(plot_frame1, aes(x=instances_dim, y=time, fill=sub_algorithm)) +
  geom_area(colour="black", lwd=.4, alpha=.7) +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,5000,500)) +
  ggtitle("Tempi medi degli algoritmi (seriale)") +
  labs(x="Dimensione instanze", y="Tempi (ms)", fill = 'Algoritmi')
plot2 <- ggplot(plot_frame2, aes(x=instances_dim, y=time, fill=sub_algorithm)) +
  geom_area(colour="black", lwd=.4, alpha=.7) +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,5000,500)) +
  ggtitle("Tempi medi degli algoritmi (parallelo)") +
  labs(x="Dimensione instanze", y="Tempi (ms)", fill = 'Algoritmi')
grid.arrange(plot1, plot2, nrow=2, ncol=1)


#COMPARAZIONE 2 - BUILD GRAPH
serial_data <- data.frame()
serial_paths <- list.files(path = "C:/Users/ricca/Desktop/Architetture parallele/Parallel-solution-for-finding-all-solutions-to-the-stable-marriage-problem/time_charts/serial", pattern = ".txt", full.names = TRUE)
temp <- serial_paths[9]
for (i in 9:2) {
  serial_paths[i] <- serial_paths[i-1] 
}
serial_paths[1] <- temp
serial_paths

parallel_data <- data.frame()
parallel_paths <- list.files(path = "C:/Users/ricca/Desktop/Architetture parallele/Parallel-solution-for-finding-all-solutions-to-the-stable-marriage-problem/time_charts/parallel", pattern = ".txt", full.names = TRUE)
temp <- parallel_paths[9]
for (i in 9:2) {
  parallel_paths[i] <- parallel_paths[i-1] 
}
parallel_paths[1] <- temp
parallel_paths

#reading files and computing means
tot_mean <- data.frame()
for (i in 1:10) {
  serial_data <- read.table(serial_paths[i], header=TRUE, sep=" ")
  parallel_data <- read.table(parallel_paths[i], header=TRUE, sep=" ")
  parallel_data <- parallel_data[-c(1),]
  r <- c(500*i, mean(serial_data$BuildGraph), mean(parallel_data$KernelOverhead))
  tot_mean <- rbind(tot_mean, r)
}
colnames(tot_mean) <- c("instance_dim", "serial_tot_mean", "parallel_tot_mean")
tot_mean

#plotting
library(ggplot2)
plot <- ggplot(tot_mean, aes(x = instance_dim))
plot +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,5000,500)) +
  ggtitle("Comparazione versione seriale e parallela di Build Graph") +
  labs(x="Dimensione instanze", y="Tempi (ms)") +
  geom_line(aes(y = serial_tot_mean, color = 'Seriale'), lwd=1) + 
  geom_line(aes(y = parallel_tot_mean, color = 'Parallela'), lwd=1) +
  scale_color_manual('Versioni', values=c('red', 'steelblue'))