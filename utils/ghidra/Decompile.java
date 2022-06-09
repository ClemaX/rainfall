import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;

import ghidra.app.script.GatherParamPanel;
import ghidra.app.script.GhidraScript;
import ghidra.app.util.Option;
import ghidra.app.util.exporter.CppExporter;

public class Decompile extends GhidraScript {
	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();

		System.out.println("ARGS: " + Arrays.toString(args));

		// File outputFile = (File) state.getEnvironmentVar("COutputFile");
		File outputFile = new File(args[0]);

		CppExporter cppExporter = new CppExporter();
		List<Option> options = new ArrayList<Option>();
		options.add(new Option(CppExporter.CREATE_HEADER_FILE, new Boolean(false)));
		cppExporter.setOptions(options);
		cppExporter.setExporterServiceProvider(state.getTool());
		cppExporter.export(outputFile, currentProgram, null, monitor);
	}
}
