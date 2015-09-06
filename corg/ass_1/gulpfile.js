var gulp = require('gulp'),
	changed = require('gulp-changed'),
	filter = require('gulp-filter'),
	latex = require('gulp-latex')

var paths = {
	sources: ['ass.tex'],
	dest: 'compiled'
}

gulp.task('latex', function() {
	return gulp.src('ass.tex')
		.pipe(changed(paths.dest))
		.pipe(latex())
		.pipe(gulp.dest(paths.dest))
		.pipe(filter('*.pdf'))
		.pipe(gulp.dest('./'))
})

gulp.task('watch', function() {
	return gulp.watch(paths.sources, ['latex'])
})

gulp.task('default', ['watch', 'latex'])
