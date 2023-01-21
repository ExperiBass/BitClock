const { SerialPort } = require('serialport')
const { get } = require('axios')
const numeral = require('numeral')

const explorer_url = "https://bitcoinexplorer.org/api"

function getBlocksTillHalving(fromHeight) {
    const HALVING_PERIOD = 210000 // 210k blok
    // there has to be a better way to do this, right?
    const nextHalving = (parseInt(fromHeight / HALVING_PERIOD) + 1) * HALVING_PERIOD
    return numeral(nextHalving - fromHeight).format()
}

function send(data) {
    return new Promise(res => {
        bitclock.write(data)
        bitclock.drain(res)
    })
}

// Create a port
const bitclock = new SerialPort({
    path: '/dev/ttyACM0',
    baudRate: 19200
})

bitclock.on('open', () => {
    console.log("Connected.")
})
bitclock.on('data', (data) => {
    const parsed = data.toString('hex')
    console.log('Data:', parsed)
    if (parsed === '0b') { // duino sends 0b when ready
        // duino is ready
        fetchAndSendData()
        // MAYBETODO: infinitely poll api (1req/5min)?
    }
})
bitclock.on('error', (e) => {
    console.error(e)
})
bitclock.on('close', () => {
    console.log("Stream closed.")
    process.exit()
})

async function fetchAndSendData() {
    try {
        const currentHeight = (await get(`${explorer_url}/blocks/tip/height`)).data
        const btcPrice = (await get(`${explorer_url}/price/usd?format=true`)).data
        const satsPrice = (await get(`${explorer_url}/price/usd/sats`)).data
        const coinsInCirculation = parseInt((await get(`${explorer_url}/blockchain/coins`)).data) // we dont want the decimals

        // format data for transfer
        const dataString = `${numeral(currentHeight).format()}H${btcPrice}P${satsPrice}S${getBlocksTillHalving(currentHeight)}B${numeral(coinsInCirculation).format()}`
        await send(dataString)
        console.log(`Wrote "${dataString}" to client.`)
    } catch(e) {
        console.error(e)
    }
}

process.on('beforeExit', () => {
    bitclock.close()
})