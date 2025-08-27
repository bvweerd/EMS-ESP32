import { useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';
import { Button, Grid, Checkbox } from '@mui/material';

import * as WireGuardApi from 'api/wireguard';

import type { ValidateFieldsError } from 'async-validator';
import {
  BlockFormControlLabel,
  BlockNavigation,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedPasswordField,
  ValidatedTextField,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { WireGuardSettingsType } from 'types';
import { updateValueDirty, useRest } from 'utils';
import { createWireGuardSettingsValidator, validate } from 'validators';

const WireGuardSettings = () => {
  const {
    loadData,
    saving,
    data,
    updateDataValue,
    origData,
    dirtyFlags,
    setDirtyFlags,
    blocker,
    saveData,
    errorMessage
  } = useRest<WireGuardSettingsType>({
    read: WireGuardApi.readWireGuardSettings,
    update: WireGuardApi.updateWireGuardSettings
  });

  const { LL } = useI18nContext();
  useLayoutTitle('WireGuard');

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValueDirty(
    origData,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue
  );

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(createWireGuardSettingsValidator(data), data);
        await saveData();
      } catch (error) {
        setFieldErrors(error as ValidateFieldsError);
      }
    };

    return (
      <>
        <BlockFormControlLabel
          control={
            <Checkbox
              name="enabled"
              checked={data.enabled}
              onChange={updateFormValue}
            />
          }
          label={LL.ENABLE_WIREGUARD()}
        />
        <Grid container spacing={2} rowSpacing={0}>
          <Grid>
            <ValidatedPasswordField
              fieldErrors={fieldErrors}
              name="private_key"
              label={LL.PRIVATE_KEY()}
              variant="outlined"
              value={data.private_key}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="peer_public_key"
              label={LL.PEER_PUBLIC_KEY()}
              variant="outlined"
              value={data.peer_public_key}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="preshared_key"
              label={LL.PRESHARED_KEY()}
              variant="outlined"
              value={data.preshared_key}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="address"
              label={LL.ADDRESS()}
              variant="outlined"
              value={data.address}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="netmask"
              label={LL.NETMASK()}
              variant="outlined"
              value={data.netmask}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="endpoint"
              label={LL.ENDPOINT()}
              variant="outlined"
              value={data.endpoint}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="port"
              label={LL.PORT()}
              variant="outlined"
              value={data.port}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="persistent_keepalive"
              label={LL.PERSISTENT_KEEPALIVE()}
              variant="outlined"
              value={data.persistent_keepalive}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
        </Grid>
        {dirtyFlags && dirtyFlags.length !== 0 && (
          <ButtonRow>
            <Button
              startIcon={<CancelIcon />}
              disabled={saving}
              variant="outlined"
              color="secondary"
              type="submit"
              onClick={loadData}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<WarningIcon color="warning" />}
              disabled={saving}
              variant="contained"
              color="info"
              type="submit"
              onClick={validateAndSubmit}
            >
              {LL.APPLY_CHANGES(dirtyFlags.length)}
            </Button>
          </ButtonRow>
        )}
      </>
    );
  };

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {content()}
    </SectionContent>
  );
};

export default WireGuardSettings;

