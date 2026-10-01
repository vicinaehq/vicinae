export class Logger {
	error(message: string) {
		this.write("error", message);
	}

	event(message: string) {
		this.write("event", message);
	}

	info(message: string) {
		this.write("info", message);
	}

	ready(message: string) {
		this.write("ready", message);
	}

	logTimestamp(s: string) {
		const ts = new Date().toJSON();
		const lines = s.split("\n");

		for (let i = 0; i !== lines.length; ++i) {
			const line = lines[i];

			if (i === lines.length - 1 && line.length === 0) continue;

			console.log(`${ts.padEnd(20)} - ${line}`);
		}
	}

	private write(level: string, message: string) {
		console.error(`${level}\t${message}`);
	}
}
