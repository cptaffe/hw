var gulp = require('gulp')
var latex = require('gulp-latex')

gulp.task('latex', function() {
	return gulp.src('ass.tex')
		.pipe(latex())
		.pipe(gulp.dest('compiled'))
})

gulp.task('default', ['latex'])
