var gulp = require('gulp'),
	latex = require('gulp-latex')

var paths = {
	sources: ['ass.tex'],
	output: 'compiled'
}

gulp.task('latex', function() {
	return gulp.src('ass.tex')
		.pipe(latex())
		.pipe(gulp.dest('compiled'))
})

gulp.task('watch', function() {
	gulp.watch(paths.sources, ['latex'])
})

gulp.task('default', ['latex'])
