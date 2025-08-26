import VpnKeyIcon from '@mui/icons-material/VpnKey';
import { Avatar, Divider, List, ListItem, ListItemAvatar, ListItemText, useTheme } from '@mui/material';
import type { Theme } from '@mui/material';

import { readWireGuardStatus } from 'api/wireguard';

import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { WireGuardStatusType } from 'types';
import { formatDateTime } from 'utils/time';
import { useInterval } from 'utils';

const statusHighlight = ({ connected }: WireGuardStatusType, theme: Theme) =>
  connected ? theme.palette.success.main : theme.palette.error.main;

const WireGuardStatus = () => {
  const { data, send: loadData, error } = useRequest(readWireGuardStatus);

  useInterval(() => {
    void loadData();
  });

  const { LL } = useI18nContext();
  useLayoutTitle('WireGuard');

  const theme = useTheme();

  const statusText = ({ enabled, connected }: WireGuardStatusType) => {
    if (!enabled) {
      return String(LL.NOT_ENABLED());
    }
    return connected ? String(LL.CONNECTED(0)) : String(LL.DISCONNECTED());
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: statusHighlight(data, theme) }}>
              <VpnKeyIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary={LL.STATUS_OF('')} secondary={statusText(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>EP</Avatar>
          </ListItemAvatar>
          <ListItemText primary="Endpoint" secondary={data.endpoint} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>IP</Avatar>
          </ListItemAvatar>
          <ListItemText primary={LL.ADDRESS_OF('IP')} secondary={data.address} />
        </ListItem>
        <Divider variant="inset" component="li" />
        {data.latest_handshake && (
          <ListItem>
            <ListItemAvatar>
              <Avatar>HS</Avatar>
            </ListItemAvatar>
            <ListItemText
              primary="Last Handshake"
              secondary={formatDateTime(new Date(data.latest_handshake * 1000))}
            />
          </ListItem>
        )}
      </List>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default WireGuardStatus;
