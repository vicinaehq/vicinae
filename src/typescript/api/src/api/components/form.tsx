import type { ReactNode, Ref } from "react";
import { useImperativeFormHandle } from "../hooks/use-imperative-form-handle";
import { type ImageLike, serializeProtoImage } from "../image";
import { Dropdown as MainDropdown } from "./dropdown";

/**
 * @category UI Components
 */
export namespace Form {
	export type Props = {
		actions?: React.ReactNode;
		children?: React.ReactNode;
		enableDrafts?: boolean;
		isLoading?: boolean;
		navigationTitle?: string;
		searchBarAccessory?: React.ReactNode;
	};

	export type TextField = FormItemRef;
	export type PasswordField = FormItemRef;
	export type TextArea = FormItemRef;
	export type Checkbox = FormItemRef;
	export type DatePicker = FormItemRef;
	export type Dropdown = FormItemRef;
	export type TagPicker = FormItemRef;
	export type FilePicker = FormItemRef;
	export type ItemReference = FormItemRef;
	type WithFormRef<T> = {
		ref?: Ref<T>;
	};

	export type ItemProps<T extends Value> = FormItemProps<T>;

	export namespace TextField {
		export type Props = FormItemProps<string> &
			WithFormRef<Form.TextField> & { placeholder?: string };
	}

	export namespace PasswordField {
		export type Props = FormItemProps<string> &
			WithFormRef<Form.PasswordField> & { placeholder?: string };
	}

	export namespace TextArea {
		export type Props = FormItemProps<string> &
			WithFormRef<Form.TextArea> & { placeholder?: string };
	}

	export namespace Checkbox {
		export type Props = FormItemProps<boolean> &
			WithFormRef<Form.Checkbox> & { label?: string };
	}

	export namespace DatePicker {
		export type Props = FormItemProps<Date | null> &
			WithFormRef<Form.DatePicker> & {
				min?: Date;
				max?: Date;
				type?: DatePickerType;
			};
	}

	export namespace Dropdown {
		export type Props = FormItemProps<string> &
			WithFormRef<Form.Dropdown> & {
				tooltip?: string;
				children?: ReactNode;
				filtering?: boolean;
				isLoading?: boolean;
				placeholder?: string;
				throttle?: boolean;
				onSearchTextChange?: (text: string) => void;
			};
	}

	export namespace TagPicker {
		export type Props = FormItemProps<string[]> &
			WithFormRef<Form.TagPicker> & { children?: ReactNode };

		export namespace Item {
			export type Props = {
				title: string;
				value: string;
				icon: ImageLike;
			};
		}
	}

	export namespace FilePicker {
		export type Props = FormItemProps<string[]> &
			WithFormRef<Form.FilePicker> & {
				allowMultipleSelection?: boolean;
				canChooseDirectories?: boolean;
				canChooseFiles?: boolean;
				showHiddenFiles?: boolean;
			};
	}

	export namespace Description {
		export type Props = {
			text: string;
			title?: string;
		};
	}

	export namespace LinkAccessory {
		export type Props = {
			target: string;
			text: string;
		};
	}

	export type Value =
		| string
		| number
		| boolean
		| string[]
		| number[]
		| Date
		| null;

	export type Values = {
		[itemId: string]: Value;
	};
}

type FormItemRef = {
	focus: () => void;
	reset: () => void;
};

interface FormItemProps<T extends Form.Value> {
	id: string;
	title?: string;
	info?: string;
	error?: string;
	storeValue?: boolean;
	autoFocus?: boolean;
	value?: T;
	defaultValue?: T;
	onChange?: (newValue: T) => void;
	onBlur?: (event: FormEvent<T>) => void;
	onFocus?: (event: FormEvent<T>) => void;
}

interface FormEvent<T extends Form.Value> {
	target: {
		id: string;
		value?: T;
	};
	type: FormEventType;
}

type FormEventType = "focus" | "blur";

const wrapFormItemProps = <T extends Form.Value>(props: FormItemProps<T>) => {
	return {
		...props,
		onFocus: () =>
			props.onFocus?.({
				type: "focus",
				target: {
					id: props.id,
					value: (props.value ?? props.defaultValue) as T | undefined,
				},
			}),
		onBlur: () =>
			props.onBlur?.({
				type: "blur",
				target: {
					id: props.id,
					value: (props.value ?? props.defaultValue) as T | undefined,
				},
			}),
	};
};

const FormRoot: React.FC<Form.Props> = ({
	enableDrafts = false,
	actions,
	children,
	isLoading = false,
	navigationTitle,
	searchBarAccessory,
}) => {
	return (
		<form
			//@ts-ignore
			enableDrafts={enableDrafts}
			isLoading={isLoading}
			navigationTitle={navigationTitle}
		>
			{searchBarAccessory}
			{children}
			{actions}
		</form>
	);
};

const TextField: React.FC<Form.TextField.Props> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <text-field {...wrapFormItemProps(props)} />;
};

const PasswordField: React.FC<Form.PasswordField.Props> = ({
	ref,
	...props
}) => {
	useImperativeFormHandle(ref);

	return <password-field {...props} />;
};

export enum DatePickerType {
	DateTime = "dateTime",
	Date = "date",
}

const DatePickerRoot: React.FC<Form.DatePicker.Props> = ({
	ref,
	onChange,
	...props
}) => {
	useImperativeFormHandle(ref);

	const _onChange = onChange
		? (newValue: string | null) => {
			const dateObj = newValue ? new Date(newValue) : null;
			onChange(dateObj);
		}
		: undefined;

	return (
		<date-picker-field {...wrapFormItemProps(props)} onChange={_onChange} />
	);
};

const DatePicker = Object.assign(DatePickerRoot, {
	Type: DatePickerType,
	isFullDay: (value: Date | null | undefined) => {
		if (!value) return false;
		return (
			value.getHours() === 0 &&
			value.getMinutes() === 0 &&
			value.getSeconds() === 0 &&
			value.getMilliseconds() === 0
		);
	},
});

const Checkbox: React.FC<Form.Checkbox.Props> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <checkbox-field {...wrapFormItemProps(props)} />;
};

//FIXME: we probably need to reuse the existing dropdown in
// a smarter way.
const DropdownRoot: React.FC<Form.Dropdown.Props> = ({
	ref,
	children,
	...props
}) => {
	// FIXME: testing stuff, we need to generalize this to all form items
	useImperativeFormHandle(ref);

	return (
		<dropdown-field {...wrapFormItemProps(props)}>{children}</dropdown-field>
	);
};

const Dropdown = Object.assign(DropdownRoot, {
	Item: MainDropdown.Item,
	Section: MainDropdown.Section,
});

const TagPickerRoot: React.FC<Form.TagPicker.Props> = ({
	children,
	...props
}) => {
	return (
		<tag-picker-field {...wrapFormItemProps(props)}>
			{children}
		</tag-picker-field>
	);
};

const TagPickerItem: React.FC<Form.TagPicker.Item.Props> = ({
	icon,
	...props
}) => {
	const serializedIcon = icon ? serializeProtoImage(icon) : icon;
	return <tag-picker-item {...props} icon={serializedIcon} />;
};

const TagPicker = Object.assign(TagPickerRoot, {
	Item: TagPickerItem,
});

const TextArea: React.FC<Form.TextArea.Props> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <text-area-field {...wrapFormItemProps(props)} />;
};

const FilePicker: React.FC<Form.FilePicker.Props> = ({ ref, ...props }) => {
	useImperativeFormHandle(ref);

	return <file-picker-field {...wrapFormItemProps(props)} />;
};

const Description: React.FC<Form.Description.Props> = (props) => {
	return <form-description {...props} />;
};

/**
 * @category UI Components
 */
export const Form = Object.assign(FormRoot, {
	TextField,
	PasswordField,
	DatePicker,
	Checkbox,
	TextArea,
	Dropdown,
	Description,
	TagPicker,
	FilePicker,
	Separator: () => <separator />,
	LinkAccessory: ({ target, text }: Form.LinkAccessory.Props) => (
		<link-accessory target={target} text={text} />
	),
});
