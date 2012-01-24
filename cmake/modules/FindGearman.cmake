find_path(GEARMAN_INCLUDE_DIR libgearman/gearman.h)

find_library(GEARMAN_LIBRARY NAMES gearman)

set(GEARMAN_LIBRARIES ${GEARMAN_LIBRARY})
set(GEARMAN_INCLUDE_DIRS ${GEARMAN_INCLUDE_DIR})

find_package_handle_standard_args(Gearman REQUIRED_VARS GEARMAN_LIBRARY GEARMAN_INCLUDE_DIR)
