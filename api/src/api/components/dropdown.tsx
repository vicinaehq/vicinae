import { ReactNode } from "react";
import { Image, serializeImageLike } from "../image";
import { useEventListener } from "../hooks";

export declare namespace Dropdown {
  type Props = DropdownProps;
  type Item = DropdownItemProps;
  type Section = DropdownSectionProps;
}

type DropdownProps = {
  tooltip?: string;
  children?: ReactNode;
  defaultValue?: string;
  filtering?: boolean;
  id?: string;
  isLoading?: boolean;
  placeholder?: string;
  storeValue?: boolean;
  throttle?: boolean;
  value?: string;
  onChange?: (newValue: string) => void;
  onSearchTextChange?: (text: string) => void;
};

type DropdownItemProps = {
  title: string;
  value: string;
  icon?: Image.ImageLike;
  keywords?: string[];
};

type DropdownSectionProps = {
  title?: string;
  children?: ReactNode;
};

const DropdownRoot: React.FC<DropdownProps> = ({ children, ...props }) => {
  const onSearchTextChange = useEventListener(props.onSearchTextChange);
  const onChange = useEventListener(props.onChange);

  return (
    <dropdown onSearchTextChange={onSearchTextChange} onChange={onChange}>
      {children}
    </dropdown>
  );
};

const Item: React.FC<DropdownItemProps> = ({ title, value, icon }) => {
  return (
    <dropdown-item
      title={title}
      value={value}
      icon={icon && serializeImageLike(icon)}
    />
  );
};

const Section: React.FC<DropdownSectionProps> = ({ title, children }) => {
  return <dropdown-section title={title}>{children}</dropdown-section>;
};

export const Dropdown = Object.assign(DropdownRoot, {
  Item,
  Section,
});
