import { Action, ActionPanel, AI, Form, getPreferenceValues, Icon, List, showToast, Toast, useNavigation } from "@omnicast/api"
import { useEffect, useMemo, useRef, useState } from "react";
import { Fruit, fruits } from "./fruits";

const CreateFruit = () => {
	const [name, setName] = useState('');
	const [nameError, setNameError] = useState("");
	const [useProxy, setUseProxy] = useState(false);

	const handleSubmit = async (values: Form.Values) => {
		console.log({ values });
	}

	return (
		<Form
			actions={
				<ActionPanel>
					<Action.SubmitForm onSubmit={handleSubmit} />
					<Action.Push title="Reopen another window" target={<CreateFruit />} />
				</ActionPanel>
			}
		>
			<Form.TextField id="name" title="Name" error={nameError} onBlur={() => console.log('blurred')} onFocus={() => console.log('focused')} onChange={(value) => { console.log({ value }); setName(value); }} value={name} />
			<Form.TextField id="description" title="Name" />
			<Form.Dropdown 
			isLoading
			onSearchTextChange={(text) => { console.log(`dropdown search: ${text}`)}}
			id="Select model" 
			value={'1'} title="Select choice" onBlur={() => console.log('blur dropdown')} onFocus={() => console.log(`focus dropdown`)}>
				<List.Dropdown.Item title={'choice1'} value={'0'} icon={Icon.Circle} />
				<List.Dropdown.Item title={'choice2'} value={'1'} icon={'https://i.pinimg.com/474x/1e/59/67/1e5967f624fb617984dbc46c8c9ff328.jpg'} />
				<List.Dropdown.Item title={'choice3'} value={'2'} icon={Icon.Circle} />
			</Form.Dropdown>
			<Form.Checkbox storeValue autoFocus id="use-proxy" title="Use proxy" value={useProxy} onChange={setUseProxy} />
			{useProxy && (
				<>
					<Form.TextField id="proxy-user" title="Username" />
					<Form.TextField id="proxy-password" title="Password" />
				</>
			)}
		</Form>
	);
}

const FruitGen = () => {
	const [isLoading, setIsLoading] = useState(false);
	const [generations, setGenerations] = useState<{[key: string]: string}>({});
	const [model, setModel] = useState<AI.ModelInfo | undefined>();
	const abortController = useRef<AbortController>(new AbortController);
	const currentGen = useRef('');

	const handleGeneration = (fruit: Fruit) => {
		const stream = AI.ask(`Write an essay about this fruit: ${fruit.name}. Include images if possible`, { signal: abortController.current.signal, model: model?.id });

		if (model) {
			showToast(Toast.Style.Success, `Generating with ${model.name}`);
		}

		setIsLoading(true);
		currentGen.current = '';

		stream.on('data', (token) => {
			currentGen.current += token;
			setGenerations({
				...generations,
				[fruit.name]: currentGen.current
			});
		});

		stream.finally(() => setIsLoading(false));
	}

	useEffect(() => {
		return () => {
			abortController.current.abort();
		}
	}, []);

	useEffect(() => {
		console.log({generations});
	}, [generations]);

	return (
		<List isShowingDetail isLoading={isLoading} searchBarAccessory={<AiModelSelector onChange={setModel} />} searchBarPlaceholder={'Search for a fruit'}>
			<List.Section title={"Fruits"}>
				{fruits.map((fruit) => (
					<List.Item 
						title={fruit.name}
						icon={fruit.emoji}
						key={fruit.name} 
						detail={generations[fruit.name] && <List.Item.Detail markdown={generations[fruit.name]} />}
						actions={
							<ActionPanel>
								<Action title="Generate AI" icon={Icon.Dna} onAction={() => handleGeneration(fruit)} />
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
};

const ControlledModeSelector = () => {
	const [search, setSearch] = useState('');
	const modes = useRef<string[]>([
		'Mode 1',
		'Mode 2',
		'Mode 3'
	]);
	const [value, setValue] = useState(modes.current[0]);
	const filteredModes = useMemo(() => modes.current.filter(mode => mode.includes(search)), [search]);

	return (
		<List.Dropdown 
			tooltip="Change mode" 
			onChange={setValue}
			onSearchTextChange={setSearch}
			value={value}
		>
			<List.Dropdown.Section title="Available modes">
				{filteredModes.map((mode, idx) => (
					<List.Dropdown.Item key={mode} title={mode} value={`${idx}`} icon={Icon.Circle} />
				))}
			</List.Dropdown.Section>
		</List.Dropdown>
	)
}

const AiModelSelector: React.FC<{ onChange?: (model: AI.ModelInfo) => void }> = ({ onChange }) => {
	const [models, setModels] = useState<AI.ModelInfo[]>([]);

	useEffect(() => {
		AI.getModels().then((models) => {
			setModels(models);
		});
	}, []);

	const handleChange = (id: string) => {
		const model = models.find(m => m.id === id);

		if (model) onChange?.(model);
	}

	return (
		<List.Dropdown 
			tooltip="Select model" 
			onChange={handleChange}
		>
			<List.Dropdown.Section title="Models">
				{models.map((model) => (
					<List.Dropdown.Item key={model.id} title={model.name} value={model.id} icon={model.icon} />
				))}
			</List.Dropdown.Section>
		</List.Dropdown>
	)
}

const FruitList = () => {
	const handleCustomCallback = (fruit: Fruit) => {
		console.log('custom callback fired with', fruit);
	}

	useEffect(() => {
		AI.getModels().then((models) => {
			console.log({ models });
		});

		console.log({ preferences: getPreferenceValues() });
	}, []);

	return (
		<List 
			isShowingDetail 
			searchBarPlaceholder={'Search for a fruit'} 
			searchBarAccessory={<ControlledModeSelector />}
			actions={
				<ActionPanel>
					<Action.Push title="Create new fruit" icon={Icon.Plus} target={<CreateFruit />} />
				</ActionPanel>
			}
		>
			<List.Section title={"Fruits"}>
				{fruits.map(fruit => (
					<List.Item 
						title={fruit.name}
						icon={fruit.emoji}
						key={fruit.name} 
						detail={
							<List.Item.Detail markdown={fruit.description} />
						}
						actions={
							<ActionPanel>
								<Action.CopyToClipboard title={"Copy to clipboard"} content={fruit.emoji} />
								<Action title="Custom callback" icon={Icon.Pencil} onAction={() => handleCustomCallback(fruit)} />
								<Action.Push title="Switch to AI gen" icon={Icon.Dna} target={<FruitGen />} />
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}

const ExampleCommand = () => {
	return <FruitList />
}


export default ExampleCommand;
