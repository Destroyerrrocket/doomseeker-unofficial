{
	// Name of the package.
	"name" : "app",

	// Files that should be included in the package.
	"files" : [
		".*"
	],

	// the name of the updater binary - this will be listed as
	// a dependency of the update process
	"updater-binary" : "updater",

	// the name of the main binary to launch when the
	// application starts
	"main-binary" : "myapp"
}
