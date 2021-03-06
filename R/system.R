### =========================================================================
### System call utilities
### -------------------------------------------------------------------------
###

getDefaultGmapPath <- function() {
  system.file("usr", "bin", package = "gmapR")
}

.commandLine.handleMissingArgs <- function(defaultArgs, userArgs) {
  ##arguments with no default need to be handled explicity, since
  ##"missing" args cannot be eval'ed.
  isMissing <- rep(NA, length(defaultArgs))
  ##using a for loop because "sapply(defaultArgs, missing)" won't work
  for (i in seq_along(isMissing)) {
    x <- defaultArgs[[i]]
    isMissing[i] <- missing(x)
  }
  handledArgs <- defaultArgs
  handledArgs[isMissing] <- "NoT_pRoViDeD"

  ##verify function calling commandLine provided values for any
  ##argument with no default
  callerMustSupply <- names(handledArgs[isMissing])
  if (!all(callerMustSupply %in% names(userArgs))) {
    stop("An argument with a required value was not passed to commandLine()")
  }

  return(handledArgs)
}

## gathers command line from non-NULL args to parent frame
commandLine <- function(binary = "gsnap",
                        path = NULL)
{
  ##get values of arguments of function that called this function
  parentFormals <- formals(sys.function(sys.parent()))
  userArgs <- mget(names(parentFormals), parent.frame())

  ##Some args cannot be passed simultaneously to gsnap. Remove arg if
  ##it is the default.
  defaultArgs <- parentFormals
  ##objects of class "name" must be handled specially since they
  ##cannot be eval'ed
  defaultArgs <- .commandLine.handleMissingArgs(defaultArgs, userArgs)
  parentEnv <- sys.parent()
  defaultArgs <- lapply(defaultArgs, eval, envir=parentEnv)  
  ##some args have a default vector with more than one element (for
  ##use with match.args). Taking first element:
  defaultArgs <- lapply(defaultArgs,
                        function(x) {
                          if (length(x) > 1L) x[[1L]] else x
                        })
  
  userArgs <- mapply(function(user, def) {
    if (!is.null(def))
      user <- as(user, class(def))
    user
  }, userArgs, defaultArgs, SIMPLIFY = FALSE)
  
  ##remove defaults
  isDefault <- mapply(identical, userArgs, defaultArgs)
  if (sum(!isDefault > 0L)) {
    userArgs <- userArgs[!isDefault]
  }

  ##handle the case where element of userArgs is missing
  userArgMissing <- rep(FALSE, length(userArgs))
  for (i in seq_along(userArgMissing)) {
    x <- userArgs[[i]]
    if (missing(x)) { userArgMissing[[i]] <- TRUE }
  }
  if (sum(userArgMissing > 0)) userArgs <- userArgs[!userArgMissing]
  
  userArgs <- Filter(Negate(is.null), userArgs)
  userArgs <- Filter(function(x) !identical(x, FALSE), userArgs)
  named <- !grepl("^\\.", names(userArgs))

  scipen <- getOption("scipen")
  options(scipen = 100) # prevent use of scientific notation
  on.exit(options(scipen = scipen))
  
  unnamedUserArgs <- sapply(userArgs[!named], as.character)
  ##long-form args have double dashes. Short-form args (single char)
  ##have single dashes.
  dashes <- ifelse(nchar(names(userArgs[named])) > 1, "--", "-")
  namedUserArgs <- paste(dashes,
                         gsub("_", "-", names(userArgs[named])),
                         sep = "")
  toggle_arg <- sapply(userArgs[named], isTRUE)
  namedUserArgs[!toggle_arg] <-
    paste(namedUserArgs[!toggle_arg],
          sapply(userArgs[named][!toggle_arg], as.character))
  ##long-form args separate arg name and value with "=". Not strictly
  ##necessary since getopts handles them, but has caused confusion so
  ##adding
  namedUserArgs[nchar(dashes) > 1] <-
    sub(" ", "=", namedUserArgs[nchar(dashes) > 1])
  .commandLine(binary, namedUserArgs, unnamedUserArgs, path = path)
}

.commandLine <- function(binary, ..., path = NULL) {
  if (is.null(path))
    path <- getOption("gmap.path", getDefaultGmapPath())
  arg.string <- paste(c(...), collapse = " ")
  paste(file.path(path, binary), arg.string)
}

.system <- function(...) {

  if (is.null(getOption("systemCallMode"))) {
    options(systemCallMode = FALSE)
  }

  sysCallModeStatus <- getOption("systemCallMode")
  if (sysCallModeStatus == TRUE) {
    error <- simpleError("system command")
    error$systemCall <- as.character(as.list(...))
    stop(error)
  } else {
    system(...)
  }
}
