import { Detail, type LaunchProps } from "@vicinae/api";

export default function ContextEcho(props: LaunchProps) {
	return (
		<Detail
			markdown={`\`\`\`json\n${JSON.stringify(props, null, 2)}\n\`\`\``}
		/>
	);
}
