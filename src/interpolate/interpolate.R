library(dplyr)

# Read data (handles -9999, NA, empty string missing values)
data <- read.csv("interpolation_input.csv", na.strings = c("", "NA", "-9999"))

# Function to interpolate missing values for one division
interpolate_division <- function(df) {
  # Sort by year
  df <- df[order(df$year), ]
  
  # Create complete year sequence (only between min/max existing years)
  complete_years <- min(df$year):max(df$year)
  
  # Build template for missing years
  template <- df[1, ]  # Copy first row as template
  template[1, ] <- NA  # Clear values
  template$id <- df$id[1]
  template$industry <- df$industry[1]
  if ("name" %in% names(df)) template$name <- "INTERPOLATED"
  if ("code" %in% names(df)) template$code <- "INTERP"
  
  # Add missing years
  missing_years <- setdiff(complete_years, df$year)
  if (length(missing_years) > 0) {
    for (year in missing_years) {
      new_row <- template
      new_row$year <- year
      df <- rbind(df, new_row)
    }
  }
  
  # Sort again
  df <- df[order(df$year), ]
  
  # Interpolate X1-X65 variables only
  x_cols <- grep("^X[0-9]+$", names(df))
  
  for (col in x_cols) {
    valid_idx <- !is.na(df[, col])
    if (sum(valid_idx) >= 2) {
      # Linear interpolation
      df[, col] <- approx(df$year[valid_idx], df[, col][valid_idx], 
                         xout = df$year, rule = 2)$y
      
      # Or use spline instead:
      # df[, col] <- spline(df$year[valid_idx], df[, col][valid_idx], 
      #                    xout = df$year)$y
    }
  }
  
  return(df)
}

# Apply to each division separately
result <- data %>%
  group_by(id) %>%
  do(interpolate_division(.)) %>%
  ungroup()

# Save results
write.csv(result, "interpolation_output.csv", row.names = FALSE, quote = FALSE)
