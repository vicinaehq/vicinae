import { AI, List, Action, ActionPanel, showToast, Toast } from "@vicinae/api";
import { useEffect, useRef, useState } from "react";

const DEFAULT_PROMPT = "Give me a fun fact about bananas";

const SearchSelector = (props: { onModelChange: (s: string) => void }) => {
	const [models, setModels] = useState<AI.Model[]>([]);

	useEffect(() => {
		AI.getModels().then(setModels);
	}, []);

	return (
		<List.Dropdown onChange={props.onModelChange}>
			<List.Dropdown.Item title="Default model" value="" />
			{models.map((model) => (
				<List.Dropdown.Item
					key={model.id}
					title={model.name}
					value={model.id}
				/>
			))}
		</List.Dropdown>
	);
};

export default function AIPlayground() {
	const [text, setText] = useState("");
	const [detail, setDetail] = useState("");
	const selectedModel = useRef<string | undefined>(undefined);

	const abort = useRef<AbortController | null>(null);

	const askAI = (prompt: string, options: AI.AskOptions) => {
		return AI.ask(prompt, {
			...options,
			model: selectedModel.current || undefined,
		});
	};

	const batchCalls = async (prompt: string, n: number) => {
		const pp = Array.from({ length: n }).map(() => askAI(prompt, {}));
		console.log("batch settled", await Promise.allSettled(pp));
	};

	return (
		<List
			searchText={text}
			isShowingDetail
			searchBarPlaceholder="AI..."
			onSearchTextChange={setText}
			searchBarAccessory={
				<SearchSelector
					onModelChange={(model) => {
						selectedModel.current = model;
					}}
				/>
			}
		>
			<List.Section title={"Models"}></List.Section>
			<List.Item
				title="Try AI.ask"
				detail={<List.Item.Detail markdown={detail} />}
				actions={
					<ActionPanel>
						<Action
							title="AI.ask"
							onAction={async () => {
								if (abort.current) {
									showToast(Toast.Style.Failure, "Already generating...");
									return;
								}

								setText("");
								setDetail("");
								abort.current = new AbortController();
								await showToast(Toast.Style.Animated, "Generating...");
								try {
									const ask = askAI(text || DEFAULT_PROMPT, {
										signal: abort.current.signal,
									});
									ask.on("data", (chunk) => {
										setDetail((detail) => detail + chunk);
									});
									console.log({ answer: await ask });
									await showToast(Toast.Style.Success, "Generated");
								} catch (error) {
									await showToast(Toast.Style.Failure, "Failed", String(error));
								} finally {
									abort.current = null;
								}
							}}
						/>
						<Action title="Cancel" onAction={() => abort.current?.abort()} />
						<Action
							title="Batch calls"
							onAction={() => batchCalls(text || DEFAULT_PROMPT, 10)}
						/>
					</ActionPanel>
				}
			/>
		</List>
	);
}
