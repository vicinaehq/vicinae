import { Detail, type LaunchProps } from "@vicinae/api";

export default function ContextEcho({ launchContext = {} }: LaunchProps) {
	return (
		<Detail
			markdown={`\`\`\`json\n${JSON.stringify(launchContext, null, 2)}\n\`\`\``}
		/>
	);
}
