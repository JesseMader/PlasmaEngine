const fs = require('fs');
const isInstalled = require('is-program-installed');
const regedit = require('regedit');
let vsRegex = new RegExp(/1[6-9]\.\d+/g);

function prereqs() {
	switch (process.platform) {
		case "win32":
			regedit.list('HKCU\\SOFTWARE\\Microsoft\\VisualStudio', function (err, result) {
				preInstallValidation(result['HKCU\\SOFTWARE\\Microsoft\\VisualStudio'].keys.some(e => vsRegex.test(e)));
			});
			break;
		case "darwin":
			log.error("NOT SUPPORTED");
			break;
		default:
			log.fatal(`PREINSTALL VALIDATION FAILED | OS ${process.platform} IS NOT SUPPORTED`);
			break;
	}
}

function preInstallValidation(vs) {
	let cmakeCHK = isInstalled('cmake.exe');

	if(vs){
		log.debug('Visual Studio 2019 is Installed');
	}else{
		log.error(`Visual Studio 2019 is NOT Installed\nVisual Studio 2019 can be downloaded from https://visualstudio.microsoft.com/downloads/`);
	}

	if(cmakeCHK){
		log.debug('CMake is Installed');
	}else{
		log.error(`CMake is NOT Installed\nYou can download CMake from https://cmake.org/download/`);
	}

	if (cmakeCHK && vs) {
		log.debug("All Requirements Installed");
	} else {
		log.error("Not All Requirements Were Met");
		process.exit(1);
	}
}


const VER = "1.0.0";
const FILE_LOCATION = "NodeBuilder/";
const FILE_NAME = "DebugLogger";
const FILE_TYPE = ".log";
const FILE_DIR = FILE_LOCATION + FILE_NAME + FILE_TYPE;
const LOG_PRNT = true;
const ERR_STCK = false;
let logo = `

\t██████╗ ██╗      █████╗ ███████╗███╗   ███╗ █████╗ 
\t██╔══██╗██║     ██╔══██╗██╔════╝████╗ ████║██╔══██╗
\t██████╔╝██║     ███████║███████╗██╔████╔██║███████║
\t██╔═══╝ ██║     ██╔══██║╚════██║██║╚██╔╝██║██╔══██║
\t██║     ███████╗██║  ██║███████║██║ ╚═╝ ██║██║  ██║
\t╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝
`;

let startArgs = process.argv.slice(2);

switch (startArgs[0]) {
	case '--init':
		logFileSync();
		prereqs();
		break;
	case '--config':
		console.log("Parameter Not Set");
		break;
	default:
	//Do Nothing
}

const log = {
	info: function info(message, tag) {
		console.log(`${dt()} [\x1b[32mINFO \x1b[0m] [${(tag)?tag:"Logger"}]\t${message}`);
		logAppend('INFO ',(tag)?tag:"Logger", message);
	},
	debug: function debug(message, tag) {
		console.debug(`${dt()} [\x1b[34mDEBUG\x1b[0m] [${(tag)?tag:"Logger"}]\t${message}`);
		logAppend('DEBUG',(tag)?tag:"Logger", message);
	},
	warn: function warn(message, tag) {
		console.warn(`${dt()} [\x1b[33mWARN \x1b[0m] [${(tag)?tag:"Logger"}]\t${message}`);
		logAppend('WARN ',(tag)?tag:"Logger", message);
	},
	error: function error(message, tag) {
		if (ERR_STCK) {
			let stack_e = createStack(new Error().stack);
			console.error(`${dt()} [\x1b[0;31mERROR\x1b[0m] [${(tag)?tag:"Logger"}]\t${message}\r\n${stack_e}`);
			logAppend('ERROR',(tag)?tag:"Logger", message, stack_e);
		} else {
			console.error(`${dt()} [\x1b[0;31mERROR\x1b[0m] [${(tag)?tag:"Logger"}]\t${message}`);
			logAppend('ERROR',(tag)?tag:"Logger", message);
		}
	},
	fatal: function fatal(message, tag) {
		let stack_f = createStack(new Error().stack);
		console.error(`${dt()} [\x1b[31mFATAL\x1b[0m] [${(tag)?tag:"Logger"}]\t\x1b[31m${message}\r\n${stack_f}\x1b[0m`);
		logAppend('FATAL',(tag)?tag:"Logger", message, stack_f);
	}
};

let dt = () => {
	let dt_out = new Date().toISOString().split('T');
	dt_out = dt_out[0] + " " + dt_out[1];
	return dt_out;
};

function createStack(stack) {
	var stackSlice = stack.replace(/^.+\n.+\n/g, '');
	stackSlice = stackSlice.replace(/\)/g, '');
	stackSlice = stackSlice.replace(/(?=\().+(?=\/.+\/)/g, '');
	stackSlice = stackSlice.replace(/chrome.+:\/\/\w+/g, '');
	return stackSlice;
}

function logAppend(lvl, tag, msg, stack) {
	if (!LOG_PRNT) { return; }
	let trace = (typeof stack == 'undefined') ? "" : `${stack}\r\n`;
	let out = `${dt()} [${lvl}] [${tag}]\t${msg}\r\n${trace}`;
	fs.appendFile(FILE_DIR, out, function (err) { if (err) throw err; });
}

async function logFileSync() {
	if (!LOG_PRNT) { return; }
	fs.unlink(FILE_DIR, function (err) { if (err) return console.error(err); });
	fs.stat(FILE_DIR, function (err, stats) {
		if (err) { fs.createWriteStream(FILE_DIR).end(); }
		console.log(`\x1b[36m${logo}\x1b[0m
\x1b[36m\tVersion:\x1b[0m ${VER}\t\t|\t\x1b[36mOutput:\x1b[0m ${FILE_NAME + FILE_TYPE}

\x1b[36mOS:\x1b[0m ${process.env.OS}
\x1b[36mArch:\x1b[0m ${process.arch}
\x1b[36mNode Version:\x1b[0m ${process.version}
\x1b[36mProcessor Architecture:\x1b[0m ${process.env.PROCESSOR_ARCHITECTURE}
\x1b[36mProcessor Name:\x1b[0m ${process.env.PROCESSOR_IDENTIFIER}
	
`);
fs.appendFile(FILE_DIR, `${logo}
Version: ${VER}
Output:${FILE_DIR}
OS: ${process.env.OS}
Arch: ${process.arch}
Node Version: ${process.version}
Processor Architecture: ${process.env.PROCESSOR_ARCHITECTURE}
Processor Name: ${process.env.PROCESSOR_IDENTIFIER}

`,
function (err) { if (err) throw err; });		
	});
	return true;
}

module.exports = {
	log
};
