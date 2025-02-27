using Microsoft.AspNetCore.Mvc;

namespace DataApi.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class InvoiceController : ControllerBase
    {
        private readonly ILogger<InvoiceController> _logger;
        private readonly DataSingleton _dataSingleton;

        public InvoiceController(ILogger<InvoiceController> logger, DataSingleton dataSingleton)
        {
            _logger = logger;
            _dataSingleton = dataSingleton;
        }

        [HttpGet]
        public InvoicesViewModel Get()
        {
            return CreateInvoiceViewModel(_dataSingleton.GetInvoices());
        }

        [HttpGet]
        [Route("{date}")]
        public InvoicesViewModel Get(DateOnly date)
        {
            return CreateInvoiceViewModel(_dataSingleton.GetInvoices().Where(invoice => invoice.Date.Equals(date)));
        }

        [HttpGet]
        [Route("total/amount")]
        public double GetTotalAmount() {
            return _dataSingleton.GetInvoices().Sum(invoice => invoice.InvoiceLines.Sum(invoiceLine => invoiceLine.Amount));
        }

        private InvoicesViewModel CreateInvoiceViewModel(IEnumerable<Invoice> invoices)
        {
            return new(invoices.First().Date, invoices.Last().Date, invoices.Count(), invoices);
        }
    }
}
