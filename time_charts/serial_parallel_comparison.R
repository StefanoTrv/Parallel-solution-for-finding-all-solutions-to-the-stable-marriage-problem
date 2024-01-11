library(ggplot2)

#COMPARAZIONE 1 - PARTE SERIALE E PARALLELA
serial_data <- data.frame()
serial_paths <- list.files(path = "Parallel-solution-for-finding-all-solutions-to-the-stable-marriage-problem/time_charts/serial", pattern = ".txt", full.names = TRUE)
temp <- serial_paths[9]
for (i in 9:2) {
  serial_paths[i] <- serial_paths[i-1] 
}
serial_paths[1] <- temp
serial_paths

parallel_data <- data.frame()
parallel_paths <- list.files(path = "Parallel-solution-for-finding-all-solutions-to-the-stable-marriage-problem/time_charts/parallel", pattern = ".txt", full.names = TRUE)
temp <- parallel_paths[9]
for (i in 9:2) {
  parallel_paths[i] <- parallel_paths[i-1] 
}
parallel_paths[1] <- temp
parallel_paths

#reading files and computing means for each test
means_serial <- data.frame()
means_parallel <- data.frame()
for (i in 1:10) {
  serial_data <- read.table(serial_paths[i], header=TRUE, sep=" ")
  serial_data <- serial_data[-c(1),]
  parallel_data <- read.table(parallel_paths[i], header=TRUE, sep=" ")
  parallel_data <- parallel_data[-c(1),]
  mean_serial <- c(mean(serial_data$GaleShapley), 
            mean(serial_data$FindAllRotations), 
            mean(serial_data$BuildGraph), 
            mean(serial_data$RecursiveSearch))
  mean_parallel <- c(mean(parallel_data$GaleShapley), 
                     mean(parallel_data$FindAllRotations), 
                     mean(parallel_data$KernelOverhead), 
                     mean(parallel_data$RecursiveSearch))
  means_serial <- rbind(means_serial, mean_serial)
  means_parallel <- rbind(means_parallel, mean_parallel)
}
colnames(means_serial) <- c("gale_shapley_mean", "find_all_rotations_mean", "build_graph_mean", "recursive_search_mean")
means_serial
colnames(means_parallel) <- c("gale_shapley_mean", "find_all_rotations_mean", "build_graph_mean", "recursive_search_mean")
means_parallel

#preparing data for area plot
instances_dim <- array()
for (i in 1:10) {
  instances_dim[i] <- i*500
}
sub_algorithm <- c("Gale-Shapley", "Find All Rotations", "Build Graph", "Recursive Search")
plot_frame1 <- data.frame()
plot_frame2 <- data.frame()
for (i in 1:10) {
  for (j in 1:4) {
    r1 <- c(instances_dim[i], sub_algorithm[j], means_serial[i,j])
    r2 <- c(instances_dim[i], sub_algorithm[j], means_parallel[i,j])
    plot_frame1 <- rbind(plot_frame1, r1)
    plot_frame2 <- rbind(plot_frame2, r2)
  }
}
colnames(plot_frame1) <- c("instances_dim", "sub_algorithm", "time")
plot_frame1$instances_dim <- as.integer(plot_frame1$instances_dim)
plot_frame1$time <- as.integer(plot_frame1$time)
plot_frame1
colnames(plot_frame2) <- c("instances_dim", "sub_algorithm", "time")
plot_frame2$instances_dim <- as.integer(plot_frame2$instances_dim)
plot_frame2$time <- as.integer(plot_frame2$time)
plot_frame2

#plotting
plot1 <- ggplot(plot_frame1, aes(x=instances_dim, y=time, fill=sub_algorithm))
plot1 + 
  geom_area(colour="black", lwd=.4, alpha=.7) +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,5000,500)) +
  ggtitle("Tempi medi degli algoritmi (seriale)") +
  labs(x="Dimensione instanze", y="Tempi (ms)", fill = 'Algoritmi')
plot2 <- ggplot(plot_frame2, aes(x=instances_dim, y=time, fill=sub_algorithm))
plot2 +
  geom_area(colour="black", lwd=.4, alpha=.7) +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,5000,500)) +
  ggtitle("Tempi medi degli algoritmi (parallelo)") +
  labs(x="Dimensione instanze", y="Tempi (ms)", fill = 'Algoritmi')


#COMPARAZIONE 2 - TEMPI TOTALI E BUILD-GRAPH
#reading files and computing means
tot_build_graph <- data.frame()
tot_times <- data.frame()
for (i in 1:10) {
  serial_data <- read.table(serial_paths[i], header=TRUE, sep=" ")
  serial_data <- serial_data[-c(1),]
  parallel_data <- read.table(parallel_paths[i], header=TRUE, sep=" ")
  parallel_data <- parallel_data[-c(1),]
  r1 <- c(500*i, mean(serial_data$BuildGraph), mean(parallel_data$KernelOverhead))
  tot_build_graph<- rbind(tot_build_graph, r1)
  r2 <- c(500*i, mean(serial_data$Total), mean(parallel_data$Total))
  tot_times <- rbind(tot_times, r2)
}
colnames(tot_build_graph) <- c("instance_dim", "serial_tot_build_graph", "parallel_tot_build_graph")
tot_build_graph
colnames(tot_times) <- c("instance_dim", "serial_tot_times", "parallel_tot_times")
tot_times

#plotting
plot1 <- ggplot(tot_times, aes(x = instance_dim))
plot1 +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,5000,500)) +
  ggtitle("Tempi totali versione seriale e parallela") +
  labs(x="Dimensione instanze", y="Tempi (ms)") +
  geom_line(aes(y = serial_tot_times, color = 'Seriale'), lwd=1) + 
  geom_line(aes(y = parallel_tot_times, color = 'Parallela'), lwd=1) +
  scale_color_manual('Versioni', values=c('red', 'steelblue'))
plot2 <- ggplot(tot_build_graph, aes(x = instance_dim))
plot2 +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,5000,500)) +
  ggtitle("Build Graph versione seriale e parallela") +
  labs(x="Dimensione instanze", y="Tempi (ms)") +
  geom_line(aes(y = serial_tot_build_graph, color = 'Seriale'), lwd=1) + 
  geom_line(aes(y = parallel_tot_build_graph, color = 'Parallela'), lwd=1) +
  scale_color_manual('Versioni', values=c('red', 'steelblue'))


#COMPARAZIONE 3 - TEMPI TOTALI VS.ALGORITMO EFFETTIVO E OVERHEAD
#DA RIFARE!
tot <- data.frame()
for (i in 1:10) {
  parallel_data <- read.table(parallel_paths[i], header=TRUE, sep=" ")
  parallel_data <- parallel_data[-c(1),]
  mean_effective_algorithm <- c(mean(parallel_data$GaleShapley) + 
                     mean(parallel_data$FindAllRotations) + 
                     mean(parallel_data$Kernel) + 
                     mean(parallel_data$RecursiveSearch))
  r <- c(500*i, mean(parallel_data$Overhead), mean_effective_algorithm, mean(parallel_data$Total))
  tot <- rbind(tot, r)
}
colnames(tot) <- c("instance_dim", "mean_overhead", "mean_effective_algorithm", "mean_all")
tot

#plotting
plot1 <- ggplot(tot, aes(x = instance_dim)) +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,5000,500)) +
  ggtitle("Distinzione tra tempo totale effettivo e tempo di overhead versione parallela") +
  labs(x="Dimensione instanze", y="Tempi (ms)") +
  geom_line(aes(y = mean_all, color = 'Totale'), lwd=1) + 
  geom_line(aes(y = mean_effective_algorithm, color = 'Effettivo'), lwd=1) +
  geom_line(aes(y = mean_overhead, color = 'Overhead'), lwd=1) +
  scale_color_manual('Versioni', values=c('red', 'steelblue', 'black'))
plot1