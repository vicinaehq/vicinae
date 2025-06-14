import { Action, ActionPanel, Icon, List } from '@omnicast/api';

const items = Array.from({ length: 10_000 }, (v, idx) => idx);

export const HeavyDutyView = () => {

	return (
		<List
			isShowingDetail
			searchBarPlaceholder={'Browse local storage'}
			actions={
				<ActionPanel>
					<Action title="Add random item" icon={Icon.PlusCircle} onAction={() => {}} />
				</ActionPanel>
			}
		>
			<List.Section title={`Items (${items.length})`}>
			{items.map((idx) => (
					<List.Item 
						key={idx}
						title={`Item ${idx}`}
						icon={Icon.CodeBlock}
						actions={
							<ActionPanel>
								<Action title="Clear item" icon={Icon.Trash} onAction={() => {}} />
							</ActionPanel>
						}
						detail={
							<List.Item.Detail markdown={`Wonderful detail for ${idx}`} />
						}
					/>
			))}
			</List.Section>
		</List>
	);
};

export default HeavyDutyView;
