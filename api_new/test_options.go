package api_new

type testOptions struct {
	updateOnly  bool
	updateMode  string
	workflowDir string
	locale      string
}

type OptionSetter func(testOptions) testOptions

func UpdateOnly(value bool) OptionSetter {
	return func(options testOptions) testOptions {
		options.updateOnly = value
		return options
	}
}

func UpdateMode(value string) OptionSetter {
	return func(options testOptions) testOptions {
		options.updateMode = value
		return options
	}
}

func WorkflowDir(value string) OptionSetter {
	return func(options testOptions) testOptions {
		options.workflowDir = value
		return options
	}
}

func AlteryxLocale(value string) OptionSetter {
	return func(options testOptions) testOptions {
		options.locale = value
		return options
	}
}
