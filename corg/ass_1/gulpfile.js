var gulp = require('gulp'),
	filter = require('gulp-filter'),
	latex = require('gulp-latex')

var paths = {
	sources: ['ass.tex'],
	dest: '.'
}

gulp.task('latex', function() {
	return gulp.src('ass.tex')
		.pipe(latex())
		.pipe(filter('*.pdf'))
		.pipe(gulp.dest(paths.dest))
})

gulp.task('watch', function() {
	return gulp.watch(paths.sources, ['latex'])
})

gulp.task('default', ['latex'])
