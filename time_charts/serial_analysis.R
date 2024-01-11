data <- list()

paths <- list.files(path = "Parallel-solution-for-finding-all-solutions-to-the-stable-marriage-problem/time_charts/serial_intro", pattern = ".txt", full.names = TRUE)
temp <- paths[8]
for (i in 8:2) {
  paths[i] <- paths[i-1] 
}
paths[1] <- temp
paths

#reading files and computing means, variance and quantiles for each test
means <- data.frame()
quantiles <- data.frame()
variance <- data.frame()
for (i in 1:8) {
  content <- read.table(paths[i], header=TRUE, sep=" ")
  data[[length(data)+1]] <- content
  mean <- c(mean(content$GaleShapley), 
            mean(content$FindAllRotations), 
            mean(content$BuildGraph), 
            mean(content$RecursiveSearch))
  means <- rbind(means, mean)
  var <- c(var(content$GaleShapley), var(content$FindAllRotations), var(content$BuildGraph), var(content$RecursiveSearch))
  variance <- rbind(variance, var)
  qnt <- c(quantile(content$GaleShapley, probs=seq(0,1,1/4)),
           quantile(content$FindAllRotations, probs=seq(0,1,1/4)), 
           quantile(content$BuildGraph, probs=seq(0,1,1/4)), 
           quantile(content$RecursiveSearch, probs=seq(0,1,1/4)))
  quantiles <- rbind(quantiles, qnt)
}
colnames(means) <- c("gale_shapley_mean", "find_all_rotations_mean", "build_graph_mean", "recursive_search_mean")
means
colnames(variance) <- c("gale_shapley_var", "find_all_rotations_var", "build_graph_var", "recursive_search_var")
variance
colnames(quantiles) <- c("gs_0%", "gs_25%", "gs_50%", "gs_75%", "gs_100%",
                        "far_0%", "far_25%", "far_50%", "far_75%", "far_100%",
                        "bg_0%", "bg_25%", "bg_50%", "bg_75%", "bg_100%",
                        "rs_0%", "rs_25%", "rs_50%", "rs_75%", "rs_100%")
quantiles
quantiles_clean <- quantiles
quantiles_clean[, c(1,3,5, 6, 8, 10, 11, 13, 15, 16, 18, 20)] <- list(NULL)
quantiles_clean

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
colnames(perc) <- c("gale_shapley_perc", "find_all_rotations_perc", "build_graph_perc", "recursive_search_perc", "total_perc")
perc

#preparing data for area plot
plot_frame <- data.frame()
instances_dim <- array()
for (i in 1:8) {
  instances_dim[i] <- i*500
}
sub_algorithm <- c("Gale-Shapley", "Find All Rotations", "Build Graph", "Recursive Search")
for (i in 1:8) {
   for (j in 1:4) {
      r <- c(instances_dim[i], sub_algorithm[j], perc[i,j])
      plot_frame <- rbind(plot_frame, r)
   }
}
colnames(plot_frame) <- c("instances_dim", "sub_algorithm", "percentage")
plot_frame$instances_dim <- as.integer(plot_frame$instances_dim)
plot_frame$percentage <- as.double(plot_frame$percentage)
plot_frame

#plotting
library(ggplot2)
plot <- ggplot(plot_frame, aes(x=instances_dim, y=percentage, fill=sub_algorithm))
plot + 
  geom_area(colour="black", lwd=.4, alpha=.7) +
  theme_bw() +
  scale_x_continuous(breaks=seq(500,4000,500)) +
  ggtitle("Ripartizione degli algoritmi (seriale)") +
  labs(x="Dimensione instanze", y="Percentuale", fill = 'Algoritmi')
